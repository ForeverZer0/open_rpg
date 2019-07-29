#include "sprite.h"

VALUE rb_cSprite;

GLuint quad_vbo;
GLuint quad_vao;

#define VERTICES_COUNT 24
#define VERTICES_SIZE (sizeof(float) * VERTICES_COUNT)
#define VERTICES_STRIDE (sizeof(float) * 4)

void rpg_sprite_init(VALUE parent) {
    rb_cSprite = rb_define_class_under(parent, "Sprite", rb_cObject);

    rb_define_alloc_func(rb_cSprite, rpg_sprite_alloc);

    rb_define_method(rb_cSprite, "initialize", rpg_sprite_initialize, -1);
    rb_define_method(rb_cSprite, "dispose", rpg_sprite_dispose, -1);
    rb_define_method(rb_cSprite, "disposed?", rpg_sprite_disposed_p, 0);
    rb_define_method(rb_cSprite, "update", rpg_sprite_update, 0);

    rb_define_method(rb_cSprite, "color", rpg_sprite_get_color, 0);
    rb_define_method(rb_cSprite, "color=", rpg_sprite_set_color, 1);
    rb_define_method(rb_cSprite, "tone", rpg_sprite_get_tone, 0);
    rb_define_method(rb_cSprite, "tone=", rpg_sprite_set_tone, 1);

    rb_define_method(rb_cSprite, "x", rpg_sprite_get_x, 0);
    rb_define_method(rb_cSprite, "x=", rpg_sprite_set_x, 1);
    rb_define_method(rb_cSprite, "y", rpg_sprite_get_y, 0);
    rb_define_method(rb_cSprite, "y=", rpg_sprite_set_y, 1);
    rb_define_method(rb_cSprite, "z", rpg_sprite_get_z, 0);
    rb_define_method(rb_cSprite, "z=", rpg_sprite_set_z, 1);

    rb_define_method(rb_cSprite, "alpha", rpg_sprite_get_alpha, 0);
    rb_define_method(rb_cSprite, "alpha=", rpg_sprite_set_alpha, 1);
    rb_define_method(rb_cSprite, "opacity", rpg_sprite_get_opacity, 0);
    rb_define_method(rb_cSprite, "opacity=", rpg_sprite_set_opacity, 1);

    rb_define_method(rb_cSprite, "flash", rpg_sprite_flash, 2);
    rb_define_method(rb_cSprite, "flash_color", rpg_sprite_flash_color, 0);
    rb_define_method(rb_cSprite, "flash_duration", rpg_sprite_flash_duration, 0);

    rb_define_method(rb_cSprite, "angle", rpg_sprite_get_angle, 0);
    rb_define_method(rb_cSprite, "angle=", rpg_sprite_set_angle, 1);
    rb_define_method(rb_cSprite, "anchor", rpg_sprite_get_anchor, 0);
    rb_define_method(rb_cSprite, "anchor=", rpg_sprite_set_anchor, 1);

    rb_define_method(rb_cSprite, "bitmap", rpg_sprite_get_image, 0);
    rb_define_method(rb_cSprite, "bitmap=", rpg_sprite_set_image, 1);

    rb_define_method(rb_cSprite, "scale", rpg_sprite_get_scale, 0);
    rb_define_method(rb_cSprite, "scale=", rpg_sprite_set_scale, 1);
    rb_define_method(rb_cSprite, "scale_x", rpg_sprite_get_scale_x, 0);
    rb_define_method(rb_cSprite, "scale_x=", rpg_sprite_set_scale_x, 1);
    rb_define_method(rb_cSprite, "scale_y", rpg_sprite_get_scale_y, 0);
    rb_define_method(rb_cSprite, "scale_y=", rpg_sprite_set_scale_y, 1);

    rb_include_module(rb_cSprite, rb_mDisposable);
}

static inline void rpg_sprite_gen_vertices_inline(RPGsprite *sprite) {

    if (quad_vao == 0) {
        if (quad_vbo != 0) {
            glDeleteBuffers(1, &quad_vbo);
        }
        glGenVertexArrays(1, &quad_vao);
        glGenBuffers(1, &quad_vbo);

        glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
        float vertices[VERTICES_COUNT] = {0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                                          0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f};
        glBufferData(GL_ARRAY_BUFFER, VERTICES_SIZE, vertices, GL_STATIC_DRAW);
        glBindVertexArray(quad_vao);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, VERTICES_STRIDE, NULL);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    sprite->vao = quad_vao;
    sprite->vbo = quad_vbo;
}

static VALUE rpg_sprite_update(VALUE self) {
    RPGsprite *sprite = DATA_PTR(self);
    if (sprite->flash.duration) {
        sprite->flash.duration--;
        if (sprite->flash.duration == 0) {
            sprite->flash.color.r = 0.0f;
            sprite->flash.color.g = 0.0f;
            sprite->flash.color.b = 0.0f;
            sprite->flash.color.a = 0.0f;
        }
    }
    return self;
}

static VALUE rpg_sprite_alloc(VALUE klass) {
    RPGsprite *sprite = ALLOC(RPGsprite);
    memset(sprite, 0, sizeof(RPGsprite));
    sprite->scale.x = 1.0f;
    sprite->scale.y = 1.0f;
    sprite->alpha = 1.0f;
    sprite->model = ALLOC(RPGmatrix4x4);
    memset(sprite->model, 0, sizeof(RPGmatrix4x4));
    sprite->blend.equation = GL_FUNC_ADD;
    sprite->blend.src_factor = GL_SRC_ALPHA;
    sprite->blend.dst_factor = GL_ONE_MINUS_SRC_ALPHA;
    sprite->updated = GL_TRUE;
    rpg_sprite_gen_vertices_inline(sprite);
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

void rpg_sprite_render(RPGsprite *sprite) {
    if (sprite->bitmap && sprite->alpha > __FLT_EPSILON__) {
        glUseProgram(_program);
        // Update Model (if required)
        if (sprite->updated) {
            GLfloat sx = sprite->scale.x * sprite->bitmap->width;
            GLfloat sy = sprite->scale.y * sprite->bitmap->height;
            GLfloat cos = cosf(sprite->rotation.radians);
            GLfloat sin = sinf(sprite->rotation.radians);
            MAT4_SET(sprite->model, sx * cos, sx * sin, 0.0f, 0.0f, sy * -sin, sy * cos, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
                     0.0f, 
                     (sprite->rotation.ox * (1.0f - cos) + sprite->rotation.oy * sin) + sprite->x,
                     (sprite->rotation.oy * (1.0f - cos) - sprite->rotation.ox * sin) + sprite->y, 
                     0.0f, 1.0f);
            sprite->updated = GL_FALSE;
        }
        // Apply Shader Uniforms
        glUniform4f(_color, sprite->color.r, sprite->color.g, sprite->color.b, sprite->color.a);
        glUniform4f(_tone, sprite->tone.r, sprite->tone.g, sprite->tone.b, sprite->tone.gr);
        glUniform1f(_alpha, sprite->alpha);
        glUniform4f(_flash, sprite->flash.color.r, sprite->flash.color.g, sprite->flash.color.b, sprite->flash.color.a);
        glUniform1i(_depth, sprite->z);
        glUniformMatrix4fv(_model, 1, GL_FALSE, (float *)sprite->model);

        // Blending
        glBlendEquation(sprite->blend.equation);
        glBlendFunc(sprite->blend.src_factor, sprite->blend.dst_factor);

        // Bind Texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, sprite->bitmap->texture);
        glBindVertexArray(sprite->vao);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }
}

static VALUE rpg_sprite_initialize(int argc, VALUE *argv, VALUE self) {
    VALUE bitmap, group;
    rb_scan_args(argc, argv, "02", &bitmap, &group);
    RPGsprite *sprite = DATA_PTR(self);
    if (RTEST(bitmap)) {
        sprite->bitmap = DATA_PTR(bitmap);
    }
    if (RTEST(group)) {
        sprite->group = DATA_PTR(group);
    }
    return Qnil;
}

ATTR_READER(rpg_sprite_get_alpha, RPGsprite, alpha, DBL2NUM)
ATTR_READER(rpg_sprite_get_x, RPGsprite, x, INT2NUM)
ATTR_READER(rpg_sprite_get_y, RPGsprite, y, INT2NUM)
ATTR_READER(rpg_sprite_get_z, RPGsprite, z, INT2NUM)
ATTR_WRITER(rpg_sprite_set_z, RPGsprite, z, NUM2INT)

static VALUE rpg_sprite_set_x(VALUE self, VALUE value) {
    RPGsprite *sprite = DATA_PTR(self);
    int v = NUM2INT(value);
    if (v != sprite->x) {
        sprite->x = v;
        sprite->updated = GL_TRUE;
    }
    return value;
}

static VALUE rpg_sprite_set_y(VALUE self, VALUE value) {
    RPGsprite *sprite = DATA_PTR(self);
    int v = NUM2INT(value);
    if (v != sprite->y) {
        sprite->y = v;
        sprite->updated = GL_TRUE;
    }
    return value;
}

static VALUE rpg_sprite_get_color(VALUE self) {
    RPGsprite *sprite = DATA_PTR(self);
    return Data_Wrap_Struct(rb_cColor, NULL, NULL, &sprite->color);
}

static VALUE rpg_sprite_set_color(VALUE self, VALUE value) {
    RPGsprite *sprite = DATA_PTR(self);
    RPGcolor *color = DATA_PTR(value);
    memcpy(&sprite->color, color, sizeof(RPGcolor));
    return value;
}

static VALUE rpg_sprite_get_tone(VALUE self) {
    RPGsprite *sprite = DATA_PTR(self);
    return Data_Wrap_Struct(rb_cTone, NULL, NULL, &sprite->tone);
}

static VALUE rpg_sprite_set_tone(VALUE self, VALUE value) {
    RPGsprite *sprite = DATA_PTR(self);
    RPGtone *tone = DATA_PTR(value);
    memcpy(&sprite->tone, tone, sizeof(RPGtone));
    return value;
}

static VALUE rpg_sprite_set_alpha(VALUE self, VALUE value) {
    RPGsprite *sprite = DATA_PTR(self);
    sprite->alpha = clampf(NUM2FLT(value), 0.0f, 1.0f);
    return value;
}

static VALUE rpg_sprite_get_opacity(VALUE self) {
    RPGsprite *sprite = DATA_PTR(self);
    return INT2NUM((int)roundf(sprite->alpha * 255.0f));
}

static VALUE rpg_sprite_set_opacity(VALUE self, VALUE value) {
    RPGsprite *sprite = DATA_PTR(self);
    sprite->alpha = clampf(NUM2INT(self) / 255.0f, 0.0f, 1.0f);
    return value;
}

static VALUE rpg_sprite_flash(VALUE self, VALUE color, VALUE duration) {
    RPGsprite *sprite = DATA_PTR(self);
    RPGcolor *c = DATA_PTR(color);
    memcpy(&sprite->flash.color, c, sizeof(RPGcolor));
    sprite->flash.duration = (GLubyte)NUM2CHR(duration);
    return self;
}

static VALUE rpg_sprite_flash_duration(VALUE self) {
    RPGsprite *sprite = DATA_PTR(self);
    return INT2NUM(sprite->flash.duration);
}

static VALUE rpg_sprite_flash_color(VALUE self) {
    RPGsprite *sprite = DATA_PTR(self);
    return Data_Wrap_Struct(rb_cColor, NULL, NULL, &sprite->flash.color);
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
    sprite->updated = GL_TRUE;
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
    sprite->updated = GL_TRUE;
    return value;
}

static VALUE rpg_sprite_get_image(VALUE self) {
    RPGsprite *sprite = DATA_PTR(self);
    return sprite->bitmap ? Data_Wrap_Struct(rb_cBitmap, NULL, NULL, sprite->bitmap) : Qnil;
}

static VALUE rpg_sprite_set_image(VALUE self, VALUE value) {
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
    sprite->scale.x = fmaxf(vec->x, 0.0001f);
    sprite->scale.y = fmaxf(vec->y, 0.0001f);
    sprite->updated = GL_TRUE;
    return value;
}

static VALUE rpg_sprite_get_scale_x(VALUE self) {
    RPGsprite *sprite = DATA_PTR(self);
    return DBL2NUM(sprite->scale.x);
}

static VALUE rpg_sprite_set_scale_x(VALUE self, VALUE value) {
    RPGsprite *sprite = DATA_PTR(self);
    sprite->scale.x = fmaxf(NUM2FLT(value), 0.0001f);
    sprite->updated = GL_TRUE;
    return value;
}

static VALUE rpg_sprite_get_scale_y(VALUE self) {
    RPGsprite *sprite = DATA_PTR(self);
    return DBL2NUM(sprite->scale.y);
}

static VALUE rpg_sprite_set_scale_y(VALUE self, VALUE value) {
    RPGsprite *sprite = DATA_PTR(self);
    sprite->scale.y = fmaxf(NUM2FLT(value), 0.0001f);
    sprite->updated = GL_TRUE;
    return value;
}
