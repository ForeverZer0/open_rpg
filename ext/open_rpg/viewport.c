#include "./viewport.h"

VALUE rb_cViewport;

void rpg_viewport_init(VALUE parent) {
    rb_cViewport = rb_define_class_under(parent, "Viewport", rb_cRenderable);
    rb_define_alloc_func(rb_cViewport, rpg_viewport_alloc);
    rb_define_method(rb_cViewport, "initialize", rpg_viewport_initialize, -1);

    rb_define_method(rb_cViewport, "z=", rpg_viewport_set_z, 1);
    rb_define_method(rb_cViewport, "rect", rpg_viewport_rect, 0);
    rb_define_method(rb_cViewport, "location", rpg_viewport_location, 0);
    rb_define_method(rb_cViewport, "size", rpg_viewport_size, 0);
    rb_define_method(rb_cViewport, "inspect", rpg_viewport_inspect, 0);

    rb_define_method(rb_cViewport, "dispose", rpg_viewport_dispose, 0);
}

static VALUE rpg_viewport_dispose(VALUE self) {
    rb_call_super(0, NULL);
    RPGviewport *v = DATA_PTR(self);
    if (v->batch) {
        if (v->batch->items) {
            xfree(v->batch->items);
        }
        xfree(v->batch);
    }
    if (v->fbo) {
        glDeleteFramebuffers(1, &v->fbo);
    }
    if (v->texture) {
        glDeleteTextures(1, &v->texture);
    }
    return Qnil;
}

VALUE rpg_viewport_alloc(VALUE klass) {

    RPGviewport *vp = ALLOC(RPGviewport);
    memset(vp, 0, sizeof(RPGviewport));
    RPG_RENDER_INIT(vp->base);
    vp->base.render = rpg_viewport_render;
    vp->batch = ALLOC(RPGbatch);
    vp->base.updated = GL_TRUE;
    rpg_batch_init(vp->batch);
    return Data_Wrap_Struct(klass, NULL, RUBY_DEFAULT_FREE, vp);
}

void rpg_viewport_render(void *viewport) {
    RPGviewport *v = viewport;
    if (v->batch == NULL) {
        rb_raise(rb_eRPGError, "disposed viewport");
    }
    if (v->batch->updated) {
        rpg_batch_sort(v->batch, 0, v->batch->total - 1);
    }

    glUseProgram(_program);
    glBindFramebuffer(GL_FRAMEBUFFER, v->fbo);
    glUniformMatrix4fv(_projection, 1, GL_FALSE, (GLfloat *)&v->projection);
    glViewport(0, 0, v->rect.width, v->rect.height);
    glScissor(0, 0, v->rect.width, v->rect.height);
    glClear(GL_COLOR_BUFFER_BIT);

    RPGrenderable *obj;
    int count = rpg_batch_total(v->batch);
    for (int i = 0; i < count; i++) {
        obj = v->batch->items[i];
        obj->render(obj);
    }

    glUseProgram(_program);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glUniformMatrix4fv(_projection, 1, GL_FALSE, (GLfloat *)&projection);
    glViewport(bounds.x, bounds.y, bounds.width, bounds.height);
    glScissor(bounds.x, bounds.y, bounds.width, bounds.height);

    // Update Model (if required)
    if (v->base.updated) {
        // GLfloat sx = v->base.scale.x * v->rect.width;
        // GLfloat sy = v->base.scale.y * v->rect.height;
        // GLfloat cos = cosf(v->base.rotation.radians);
        // GLfloat sin = sinf(v->base.rotation.radians);
        // GLfloat ox = v->base.rotation.ox, oy = v->base.rotation.oy;
        // MAT4_SET(v->base.model, sx * cos, sx * sin, 0.0f, 0.0f, sy * -sin, -sy * cos, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        //          (ox * (1.0f - cos) + oy * sin) + v->rect.x, ((oy * (1.0f - cos) - ox * sin) + v->rect.y + (sx * cos)) - v->rect.height, 0.0f, 1.0f);

            GLfloat sx = v->base.scale.x * v->rect.width;
            GLfloat sy = v->base.scale.y * v->rect.height;
            GLfloat cos = cosf(v->base.rotation.radians);
            GLfloat sin = sinf(v->base.rotation.radians);
            MAT4_SET(v->base.model, sx * cos, sx * sin, 0.0f, 0.0f, sy * -sin, sy * cos, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
                     (v->base.rotation.ox * (1.0f - cos) + v->base.rotation.oy * sin) + v->rect.x,
                     (v->base.rotation.oy * (1.0f - cos) - v->base.rotation.ox * sin) + v->rect.y, 0.0f, 1.0f);

        v->base.updated = GL_FALSE;
    }
    // Apply Shader Uniforms
    glUniform4f(_color, v->base.color.r, v->base.color.g, v->base.color.b, v->base.color.a);
    glUniform4f(_tone, v->base.tone.r, v->base.tone.g, v->base.tone.b, v->base.tone.gr);
    glUniform1f(_alpha, v->base.alpha);
    glUniform4f(_flash, v->base.flash.color.r, v->base.flash.color.g, v->base.flash.color.b, v->base.flash.color.a);
    glUniformMatrix4fv(_model, 1, GL_FALSE, (float *)&v->base.model);

    // Blending
    glBlendEquation(v->base.blend.equation);
    glBlendFunc(v->base.blend.src_factor, v->base.blend.dst_factor);

    // Bind Texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, v->texture);
    glBindVertexArray(quad_vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

static VALUE rpg_viewport_set_z(VALUE self, VALUE value) {
    RPGviewport *v = DATA_PTR(self);
    int z = NUM2INT(value);
    if (v->base.z != z) {
        v->base.z = z;
        game_batch->updated = GL_TRUE;
    }
    return value;
}

static VALUE rpg_viewport_initialize(int argc, VALUE *argv, VALUE self) {
    rb_call_super(0, NULL);
    VALUE a1, a2, a3, a4;
    rb_scan_args(argc, argv, "04", &a1, &a2, &a3, &a4);
    RPGviewport *v = DATA_PTR(self);
    switch (argc) {
        case 0: {
            v->rect.x = 0;
            v->rect.y = 0;
            v->rect.width = game_width;
            v->rect.height = game_height;
            break;
        }
        case 1: {
            RPGrect *r = DATA_PTR(a1);
            memcpy(&v->rect, r, sizeof(RPGrect));
            break;
        }
        case 2: {
            RPGpoint *p = DATA_PTR(a1);
            RPGsize *s = DATA_PTR(a2);
            v->rect.x = p->x;
            v->rect.y = p->y;
            v->rect.width = s->width;
            v->rect.height = s->height;
            break;
        }
        case 4: {
            v->rect.x = NUM2INT(a1);
            v->rect.y = NUM2INT(a2);
            v->rect.width = NUM2INT(a3);
            v->rect.height = NUM2INT(a4);
            break;
        }
        default: {
            rb_raise(rb_eArgError, "wrong number of arguments (given %d, expected 0, 1, 2, 4)", argc);
        }
    }
    check_dimensions(v->rect.width, v->rect.height);
    rpg_batch_add(game_batch, &v->base);
    // Framebuffer
    glGenFramebuffers(1, &v->fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, v->fbo);
    // Texture
    glGenTextures(1, &v->texture);
    glBindTexture(GL_TEXTURE_2D, v->texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, v->rect.width, v->rect.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    // Bind texture to FBO
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, v->texture, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    MAT4_ORTHO(v->projection, 0.0f, v->rect.width, v->rect.height, 0.0f, -1.0f, 1.0f);
    return Qnil;
}

static VALUE rpg_viewport_rect(VALUE self) {
    RPGviewport *viewport = DATA_PTR(self);
    RPGrect *rect = ALLOC(RPGrect);
    memcpy(rect, &viewport->rect, sizeof(RPGrect));
    return Data_Wrap_Struct(rb_cRect, NULL, RUBY_DEFAULT_FREE, rect);
}

static VALUE rpg_viewport_location(VALUE self) {
    RPGviewport *viewport = DATA_PTR(self);
    RPGpoint *point = ALLOC(RPGpoint);
    point->x = viewport->rect.x;
    point->y = viewport->rect.y;
    return Data_Wrap_Struct(rb_cPoint, NULL, RUBY_DEFAULT_FREE, point);
}

static VALUE rpg_viewport_size(VALUE self) {
    RPGviewport *viewport = DATA_PTR(self);
    RPGsize *size = ALLOC(RPGsize);
    size->width = viewport->rect.width;
    size->height = viewport->rect.height;
    return Data_Wrap_Struct(rb_cSize, NULL, RUBY_DEFAULT_FREE, size);
}

static VALUE rpg_viewport_inspect(VALUE self) {
    RPGviewport *v = DATA_PTR(self);
    return rb_sprintf("<Viewport: x:%d y:%d width:%d height:%d>", v->rect.x, v->rect.y, v->rect.width, v->rect.height);
}