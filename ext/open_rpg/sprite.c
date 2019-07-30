#include "sprite.h"

VALUE rb_cSprite;
VALUE rb_cBlend;

void rpg_sprite_init(VALUE parent) {
    rb_cSprite = rb_define_class_under(parent, "Sprite", rb_cRenderable);
    rb_cBlend = rb_define_class_under(parent, "Blend", rb_cObject);

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

    rb_define_method(rb_cSprite, "angle", rpg_sprite_get_angle, 0);
    rb_define_method(rb_cSprite, "angle=", rpg_sprite_set_angle, 1);
    rb_define_method(rb_cSprite, "anchor", rpg_sprite_get_anchor, 0);
    rb_define_method(rb_cSprite, "anchor=", rpg_sprite_set_anchor, 1);

    rb_define_method(rb_cSprite, "bitmap", rpg_sprite_get_bitmap, 0);
    rb_define_method(rb_cSprite, "bitmap=", rpg_sprite_set_bitmap, 1);

    rb_define_method(rb_cSprite, "scale", rpg_sprite_get_scale, 0);
    rb_define_method(rb_cSprite, "scale=", rpg_sprite_set_scale, 1);
    rb_define_method(rb_cSprite, "zoom", rpg_sprite_zoom, 1);
    rb_define_method(rb_cSprite, "zoom_x", rpg_sprite_get_zoom_x, 0);
    rb_define_method(rb_cSprite, "zoom_x=", rpg_sprite_set_zoom_x, 1);
    rb_define_method(rb_cSprite, "zoom_y", rpg_sprite_get_zoom_y, 0);
    rb_define_method(rb_cSprite, "zoom_y=", rpg_sprite_set_zoom_y, 1);

    rb_define_method(rb_cSprite, "blend", rpg_sprite_get_blend, 0);
    rb_define_method(rb_cSprite, "blend=", rpg_sprite_set_blend, 1);

    #if RMXP_COMPAT
    rb_define_method(rb_cSprite, "blend_type", rpg_sprite_get_blend_type, 0);
    rb_define_method(rb_cSprite, "blend_type=", rpg_sprite_set_blend_type, 1);
    #endif

    rb_include_module(rb_cSprite, rb_mDisposable);
}

static VALUE rpg_sprite_alloc(VALUE klass) {
    RPGsprite *sprite = ALLOC(RPGsprite);
    memset(sprite, 0, sizeof(RPGsprite));
    sprite->scale.x = 1.0f;
    sprite->scale.y = 1.0f;
    sprite->model = ALLOC(RPGmatrix4x4);
    memset(sprite->model, 0, sizeof(RPGmatrix4x4));
    sprite->blend.equation = GL_FUNC_ADD;
    sprite->blend.src_factor = GL_SRC_ALPHA;
    sprite->blend.dst_factor = GL_ONE_MINUS_SRC_ALPHA;
    sprite->vao = quad_vao;
    sprite->vbo = quad_vbo;
    sprite->base.render = rpg_sprite_render;
    sprite->base.visible = GL_TRUE;
    sprite->base.alpha = 1.0f;
    sprite->base.updated = GL_TRUE;
    return Data_Wrap_Struct(klass, NULL, RUBY_DEFAULT_FREE, sprite);
}

static VALUE rpg_sprite_dispose(int argc, VALUE *argv, VALUE self) {
    VALUE bmp;
    rb_scan_args(argc, argv, "01", &bmp);
    RPGsprite *sprite = DATA_PTR(self);
    if (sprite->model) {
        xfree(sprite->model);
        sprite->model = NULL;
    }
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
    return RB_BOOL(sprite->model == NULL);
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
            GLfloat sx = s->scale.x * s->bitmap->width;
            GLfloat sy = s->scale.y * s->bitmap->height;
            GLfloat cos = cosf(s->rotation.radians);
            GLfloat sin = sinf(s->rotation.radians);
            MAT4_SET(s->model, sx * cos, sx * sin, 0.0f, 0.0f, sy * -sin, sy * cos, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
                     0.0f, 
                     (s->rotation.ox * (1.0f - cos) + s->rotation.oy * sin) + s->x,
                     (s->rotation.oy * (1.0f - cos) - s->rotation.ox * sin) + s->y, 
                     0.0f, 1.0f);
            s->base.updated = GL_FALSE;
        }
        // Apply Shader Uniforms
        glUniform4f(_color, s->base.color.r, s->base.color.g, s->base.color.b, s->base.color.a);
        glUniform4f(_tone, s->base.tone.r, s->base.tone.g, s->base.tone.b, s->base.tone.gr);
        glUniform1f(_alpha, s->base.alpha);
        glUniform4f(_flash, s->base.flash.color.r, s->base.flash.color.g, s->base.flash.color.b, s->base.flash.color.a);
        glUniform1i(_depth, s->base.z);
        glUniformMatrix4fv(_model, 1, GL_FALSE, (float *)s->model);

        // Blending
        glBlendEquation(s->blend.equation);
        glBlendFunc(s->blend.src_factor, s->blend.dst_factor);

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
    }
    else {
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

static VALUE rpg_sprite_get_angle(VALUE self) {
    RPGsprite *sprite = DATA_PTR(self);
    const float f = 180.0f / FLT_PI;
    return DBL2NUM((f * sprite->rotation.radians));
}

static VALUE rpg_sprite_set_angle(VALUE self, VALUE value) {
    RPGsprite *sprite = DATA_PTR(self);
    const float f = FLT_PI / 180.0f;
    sprite->rotation.radians = f * clampf(NUM2FLT(value), 0.0f, 360.0f);
    sprite->base.updated = GL_TRUE;
    return value;
}

static VALUE rpg_sprite_get_anchor(VALUE self) {
    RPGsprite *sprite = DATA_PTR(self);
    RPGpoint *point = ALLOC(RPGpoint);
    point->x = sprite->rotation.ox;
    point->y = sprite->rotation.oy;
    return Data_Wrap_Struct(rb_cPoint, NULL, RUBY_DEFAULT_FREE, point);
}

static VALUE rpg_sprite_set_anchor(VALUE self, VALUE value) {
    RPGsprite *sprite = DATA_PTR(self);
    RPGpoint *point = DATA_PTR(value);
    sprite->rotation.ox = point->x;
    sprite->rotation.oy = point->y;
    sprite->base.updated = GL_TRUE;
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

static VALUE rpg_sprite_get_scale(VALUE self) {
    RPGsprite *sprite = DATA_PTR(self);
    RPGvector2 *vec = ALLOC(RPGvector2);
    memcpy(vec, &sprite->scale, sizeof(RPGvector2));
    return Data_Wrap_Struct(rb_cVector2, NULL, RUBY_DEFAULT_FREE, vec);
}

static VALUE rpg_sprite_set_scale(VALUE self, VALUE value) {
    RPGsprite *sprite = DATA_PTR(self);
    RPGvector2 *vec = DATA_PTR(value);
    sprite->scale.x = vec->x;
    sprite->scale.y = vec->y;
    sprite->base.updated = GL_TRUE;
    return value;
}

static VALUE rpg_sprite_zoom(VALUE self, VALUE value) {
    RPGsprite *sprite = DATA_PTR(self);
    float scale = NUM2FLT(value);
    sprite->scale.x = scale;
    sprite->scale.y = scale;
    sprite->base.updated = GL_TRUE;
    return self;
}

static VALUE rpg_sprite_get_zoom_x(VALUE self) {
    RPGsprite *sprite = DATA_PTR(self);
    return DBL2NUM(sprite->scale.x);
}

static VALUE rpg_sprite_set_zoom_x(VALUE self, VALUE value) {
    RPGsprite *sprite = DATA_PTR(self);
    sprite->scale.x = NUM2FLT(value);
    sprite->base.updated = GL_TRUE;
    return value;
}

static VALUE rpg_sprite_get_zoom_y(VALUE self) {
    RPGsprite *sprite = DATA_PTR(self);
    return DBL2NUM(sprite->scale.y);
}

static VALUE rpg_sprite_set_zoom_y(VALUE self, VALUE value) {
    RPGsprite *sprite = DATA_PTR(self);
    sprite->scale.y = NUM2FLT(value);
    sprite->base.updated = GL_TRUE;
    return value;
}

static VALUE rpg_sprite_get_blend(VALUE self) {
    RPGsprite *sprite = DATA_PTR(self);
    RPGblend *blend = ALLOC(RPGblend);
    memcpy(blend, &sprite->blend, sizeof(RPGblend));
    return Data_Wrap_Struct(rb_cBlend, NULL, RUBY_DEFAULT_FREE, blend);
}

static VALUE rpg_sprite_set_blend(VALUE self, VALUE value) {
    RPGsprite *sprite = DATA_PTR(self);
    if (NIL_P(value)) {
        rb_raise(rb_eArgError, "value cannot be nil");
    }
    RPGblend *blend = DATA_PTR(value);
    memcpy(&sprite->blend, blend, sizeof(RPGblend));
    return value;
}

#if RMXP_COMPAT
static VALUE rpg_sprite_get_blend_type(VALUE self) {
    RPGsprite *sprite = DATA_PTR(self);
    // TODO: Implement
    return Qnil;
}

static VALUE rpg_sprite_set_blend_type(VALUE self, VALUE value) {
    RPGsprite *sprite = DATA_PTR(self);
    int type = NUM2INT(value);
    switch (type) {
        case 0: {
            sprite->blend.equation = GL_FUNC_ADD;
            sprite->blend.src_factor = GL_SRC_ALPHA;
            sprite->blend.dst_factor = GL_ONE_MINUS_SRC_ALPHA;
            break;
        }
        case 1: {
            // TODO: Implement
            break;
        }
        case 2: {
            // TODO: Implement
            break;
        }
        default: {
            rb_raise(rb_eArgError, "invalid blend type specified (given %d, expected 0, 1, 2)", type);
            break;
        }
    }
    return value;
}
#endif