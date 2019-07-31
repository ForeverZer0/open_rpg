#include "./sprite.h"

VALUE rb_cSprite;

void rpg_sprite_init(VALUE parent) {
    rb_cSprite = rb_define_class_under(parent, "Sprite", rb_cRenderable);

    rb_define_alloc_func(rb_cSprite, rpg_sprite_alloc);

    rb_define_method(rb_cSprite, "initialize", rpg_sprite_initialize, -1);
    rb_define_method(rb_cSprite, "dispose", rpg_sprite_dispose, -1);
    rb_define_method(rb_cSprite, "disposed?", rpg_sprite_disposed_p, 0);
    rb_define_method(rb_cSprite, "viewport", rpg_sprite_viewport, 0);

    rb_define_method(rb_cSprite, "x", rpg_sprite_get_x, 0);
    rb_define_method(rb_cSprite, "x=", rpg_sprite_set_x, 1);
    rb_define_method(rb_cSprite, "y", rpg_sprite_get_y, 0);
    rb_define_method(rb_cSprite, "y=", rpg_sprite_set_y, 1);
    rb_define_method(rb_cSprite, "z=", rpg_sprite_set_z, 1);

    rb_define_method(rb_cSprite, "bitmap", rpg_sprite_get_bitmap, 0);
    rb_define_method(rb_cSprite, "bitmap=", rpg_sprite_set_bitmap, 1);
}

static VALUE rpg_sprite_alloc(VALUE klass) {
    RPGsprite *sprite = ALLOC(RPGsprite);
    memset(sprite, 0, sizeof(RPGsprite));
    sprite->vao = quad_vao;
    sprite->vbo = quad_vbo;
    RPG_RENDER_INIT(sprite->base);
    sprite->base.render = rpg_sprite_render;
    return Data_Wrap_Struct(klass, NULL, RUBY_DEFAULT_FREE, sprite);
}

static VALUE rpg_sprite_dispose(int argc, VALUE *argv, VALUE self) {
    VALUE bmp;
    rb_scan_args(argc, argv, "01", &bmp);
    RPGsprite *sprite = DATA_PTR(self);
    if (sprite->vao && sprite->vao != quad_vao) {
        glDeleteVertexArrays(1, &sprite->vao);
        sprite->vao = 0;
    }
    if (sprite->vbo && sprite->vbo != quad_vbo) {
        glDeleteBuffers(1, &sprite->vbo);
        sprite->vbo = 0;
    }
    if (RTEST(bmp) && sprite->bitmap) {
        if (sprite->bitmap->texture) {
            glDeleteTextures(1, &sprite->bitmap->texture);
        }
        if (sprite->bitmap->fbo) {
            glDeleteFramebuffers(1, &sprite->bitmap->fbo);
        }
        sprite->bitmap = NULL;
    }
    return Qnil;
}

static VALUE rpg_sprite_disposed_p(VALUE self) {
    RPGsprite *sprite = DATA_PTR(self);
    // TODO: Implement
    return Qnil;
}

static VALUE rpg_sprite_viewport(VALUE self) {
    RPGsprite *sprite = DATA_PTR(self);
    if (sprite->viewport) {
        return Data_Wrap_Struct(rb_cViewport, NULL, NULL, sprite->viewport);
    }
    return Qnil;
}

void rpg_sprite_render(void *sprite) {
    RPGsprite *s = sprite;
    if (s->bitmap && s->base.visible && s->base.alpha > __FLT_EPSILON__) {
        glUseProgram(_program);
        // Update Model (if required)
        if (s->base.updated) {
            GLfloat sx = s->base.scale.x * s->bitmap->width;
            GLfloat sy = s->base.scale.y * s->bitmap->height;
            GLfloat cos = cosf(s->base.rotation.radians);
            GLfloat sin = sinf(s->base.rotation.radians);
            MAT4_SET(s->base.model, sx * cos, sx * sin, 0.0f, 0.0f, sy * -sin, sy * cos, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
                     (s->base.rotation.ox * (1.0f - cos) + s->base.rotation.oy * sin) + s->x,
                     (s->base.rotation.oy * (1.0f - cos) - s->base.rotation.ox * sin) + s->y, 0.0f, 1.0f);
            s->base.updated = GL_FALSE;
        }

        // Apply Shader Uniforms
        glUniform4f(_color, s->base.color.r, s->base.color.g, s->base.color.b, s->base.color.a);
        glUniform4f(_tone, s->base.tone.r, s->base.tone.g, s->base.tone.b, s->base.tone.gr);
        glUniform1f(_alpha, s->base.alpha);
        glUniform4f(_flash, s->base.flash.color.r, s->base.flash.color.g, s->base.flash.color.b, s->base.flash.color.a);
        glUniformMatrix4fv(_model, 1, GL_FALSE, (float *)&s->base.model);

        // Blending
        glBlendEquation(s->base.blend.equation);
        glBlendFunc(s->base.blend.src_factor, s->base.blend.dst_factor);

        // Bind Texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, s->bitmap->texture);
        glBindVertexArray(s->vao);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }
}

static VALUE rpg_sprite_initialize(int argc, VALUE *argv, VALUE self) {
    rb_call_super(0, NULL);
    VALUE viewport, bitmap;
    rb_scan_args(argc, argv, "02", &viewport, &bitmap);
    RPGsprite *sprite = DATA_PTR(self);
    if (RTEST(viewport)) {
        sprite->viewport = DATA_PTR(viewport);
        rpg_batch_add(sprite->viewport->batch, &sprite->base);
    } else {
        rpg_batch_add(game_batch, &sprite->base);
    }
    if (RTEST(bitmap)) {
        sprite->bitmap = DATA_PTR(bitmap);
    }
    return Qnil;
}

ATTR_READER(rpg_sprite_get_x, RPGsprite, x, INT2NUM)
ATTR_READER(rpg_sprite_get_y, RPGsprite, y, INT2NUM)

static VALUE rpg_sprite_set_z(VALUE self, VALUE value) {
    RPGsprite *sprite = DATA_PTR(self);
    int z = NUM2INT(value);
    if (sprite->base.z != z) {
        sprite->base.z = z;
        if (sprite->viewport) {
            sprite->viewport->batch->updated = GL_TRUE;
        } else {
            game_batch->updated = GL_TRUE;
        }
    }
    return value;
}

static VALUE rpg_sprite_set_x(VALUE self, VALUE value) {
    RPGsprite *sprite = DATA_PTR(self);
    int v = NUM2INT(value);
    if (v != sprite->x) {
        sprite->x = v;
        sprite->base.updated = GL_TRUE;
    }
    return value;
}

static VALUE rpg_sprite_set_y(VALUE self, VALUE value) {
    RPGsprite *sprite = DATA_PTR(self);
    int v = NUM2INT(value);
    if (v != sprite->y) {
        sprite->y = v;
        sprite->base.updated = GL_TRUE;
    }
    return value;
}

static VALUE rpg_sprite_get_bitmap(VALUE self) {
    RPGsprite *sprite = DATA_PTR(self);
    return sprite->bitmap ? Data_Wrap_Struct(rb_cBitmap, NULL, NULL, sprite->bitmap) : Qnil;
}

static VALUE rpg_sprite_set_bitmap(VALUE self, VALUE value) {
    RPGsprite *sprite = DATA_PTR(self);
    sprite->bitmap = NIL_P(value) ? NULL : DATA_PTR(value);
    return value;
}