#include "./sprite.h"

VALUE rb_cSprite;

void rpg_sprite_init(VALUE parent) {
    rb_cSprite = rb_define_class_under(parent, "Sprite", rb_cRenderable);
    rb_define_alloc_func(rb_cSprite, rpg_sprite_alloc);

    rb_define_method(rb_cSprite, "initialize", rpg_sprite_initialize, -1);
    rb_define_method(rb_cSprite, "viewport", rpg_sprite_viewport, 0);

    rb_define_method(rb_cSprite, "x", rpg_sprite_get_x, 0);
    rb_define_method(rb_cSprite, "x=", rpg_sprite_set_x, 1);
    rb_define_method(rb_cSprite, "y", rpg_sprite_get_y, 0);
    rb_define_method(rb_cSprite, "y=", rpg_sprite_set_y, 1);
    rb_define_method(rb_cSprite, "z=", rpg_sprite_set_z, 1);
    
    rb_define_method(rb_cSprite, "hue", rpg_sprite_get_hue, 0);
    rb_define_method(rb_cSprite, "hue=", rpg_sprite_set_hue, 1);

    rb_define_method(rb_cSprite, "image", rpg_sprite_get_image, 0);
    rb_define_method(rb_cSprite, "image=", rpg_sprite_set_image, 1);
    rb_define_method(rb_cSprite, "src_rect", rpg_sprite_get_rect, 0);
    rb_define_method(rb_cSprite, "src_rect=", rpg_sprite_set_rect, 1);

    rb_define_method(rb_cSprite, "dispose", rpg_sprite_dispose, -1);
}

static VALUE rpg_sprite_dispose(int argc, VALUE *argv, VALUE self) {
    rb_call_super(0, NULL);
    RPGsprite *s = DATA_PTR(self);
    if (s->vao != quad_vao) {
        glDeleteVertexArrays(1, &s->vao);
    }
    if (s->vbo != quad_vbo) {
        glDeleteBuffers(1, &s->vbo);
    }
    VALUE img_dispose;
    rb_scan_args(argc, argv, "01", &img_dispose);
    if (RTEST(img_dispose) && s->image) {
        if (s->image->texture) {
            glDeleteTextures(1, &s->image->texture);
            s->image->texture = 0;
        }
        if (s->image->fbo) {
            glDeleteFramebuffers(1, &s->image->fbo);
            s->image->fbo = 0;
        }
        s->image = NULL;
    }
    return Qnil;
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

static VALUE rpg_sprite_get_rect(VALUE self) {
    RPGsprite *sprite = DATA_PTR(self);
    RPGrect *rect = ALLOC(RPGrect);
    if (sprite->image) {
        memcpy(&sprite->src_rect, rect, sizeof(RPGrect));
    } else {
        memset(rect, 0, sizeof(RPGrect));
    }
    return Data_Wrap_Struct(rb_cRect, NULL, RUBY_DEFAULT_FREE, rect);
}

static VALUE rpg_sprite_set_rect(VALUE self, VALUE value) {
    RPGsprite *s = DATA_PTR(self);
    // If nil, delete existing VAO/VBO and set to none
    if (NIL_P(value)) {
        memset(&s->src_rect, 0, sizeof(RPGrect));
        if (s->vao && s->vao != quad_vao) {
            glDeleteVertexArrays(1, &s->vao);
        }
        if (s->vbo && s->vbo != quad_vbo) {
            glDeleteBuffers(1, &s->vbo);
        }
        s->vao = 0;
        s->vbo = 0;
        return Qnil;
    }

    // Return if source rect is already the specified value
    RPGrect *rect = DATA_PTR(value);
    if (s->src_rect.x == rect->x && s->src_rect.y == rect->y && s->src_rect.width == rect->width && s->src_rect.height == rect->height) {
        return value;
    }
    memcpy(&s->src_rect, rect, sizeof(RPGrect));

    GLfloat l = (GLfloat)rect->x / s->image->width;
    GLfloat t = (GLfloat)rect->x / s->image->width;
    GLfloat r = l + ((GLfloat)rect->width / s->image->width);
    GLfloat b = t + ((GLfloat)rect->height / s->image->height);

    // TODO: Create VBO is needed (GL_DYNAMIC_DRAW?)

    glGenVertexArrays(1, &s->vao);
    glBindBuffer(GL_ARRAY_BUFFER, s->vbo);
    float vertices[VERTICES_COUNT] = {0.0f, 1.0f, l, b, 1.0f, 0.0f, r, t, 0.0f, 0.0f, l, t,
                                      0.0f, 1.0f, l, b, 1.0f, 1.0f, r, b, 1.0f, 0.0f, r, t};
    glBufferData(GL_ARRAY_BUFFER, VERTICES_SIZE, vertices, GL_STATIC_DRAW);
    glBindVertexArray(s->vao);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, VERTICES_STRIDE, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    s->base.updated = GL_TRUE;
    return value;
}

static VALUE rpg_sprite_viewport(VALUE self) {
    RPGsprite *sprite = DATA_PTR(self);
    return (sprite->viewport) ? Data_Wrap_Struct(rb_cViewport, NULL, RUBY_DEFAULT_FREE, sprite->viewport) : Qnil;
}

void rpg_sprite_render(void *sprite) {
    RPGsprite *s = sprite;
    if (s->image && s->base.visible && s->base.alpha > __FLT_EPSILON__) {
        glUseProgram(_program);
        if (s->base.updated) {
            GLfloat sx = s->base.scale.x * s->src_rect.width;
            GLfloat sy = s->base.scale.y * s->src_rect.height;
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
        glUniform1f(_hue, s->hue);
        glUniform4f(_flash, s->base.flash.color.r, s->base.flash.color.g, s->base.flash.color.b, s->base.flash.color.a);
        glUniformMatrix4fv(_model, 1, GL_FALSE, (float *)&s->base.model);

        

        // Blending
        glBlendEquation(s->base.blend.equation);
        glBlendFunc(s->base.blend.src_factor, s->base.blend.dst_factor);

        // Bind Texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, s->image->texture);
        glBindVertexArray(s->vao ? s->vao : quad_vao);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }
}

static VALUE rpg_sprite_initialize(int argc, VALUE *argv, VALUE self) {
    rb_call_super(0, NULL);
    VALUE viewport, options;
    rb_scan_args(argc, argv, "01:", &viewport, &options);
    RPGsprite *sprite = DATA_PTR(self);
    if (RTEST(viewport)) {
        sprite->viewport = DATA_PTR(viewport);
        rpg_batch_add(sprite->viewport->batch, &sprite->base);
    } else {
        rpg_batch_add(game_batch, &sprite->base);
    }
    if (RTEST(options)) {
        VALUE opt = rb_hash_aref(options, STR2SYM("image"));
        if (RTEST(opt)) {
            sprite->image = DATA_PTR(opt);
            sprite->src_rect.x = 0;
            sprite->src_rect.y = 0;
            sprite->src_rect.width = sprite->image->width;
            sprite->src_rect.height = sprite->image->height;
        }
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

static VALUE rpg_sprite_get_image(VALUE self) {
    RPGsprite *sprite = DATA_PTR(self);
    return sprite->image ? Data_Wrap_Struct(rb_cImage, NULL, RUBY_DEFAULT_FREE, sprite->image) : Qnil;
}

static VALUE rpg_sprite_set_image(VALUE self, VALUE value) {
    RPGsprite *sprite = DATA_PTR(self);
    sprite->src_rect.x = 0;
    sprite->src_rect.y = 0;
    if (RTEST(value)) {
        sprite->image = DATA_PTR(value);
        sprite->src_rect.width = sprite->image->width;
        sprite->src_rect.height = sprite->image->height;
    } else {
        sprite->image = NULL;
        sprite->src_rect.width = 0;
        sprite->src_rect.height = 0;
    }
    return value;
}

static VALUE rpg_sprite_get_hue(VALUE self) {
    RPGsprite *s = DATA_PTR(self);
    return DBL2NUM(s->hue);
}

static VALUE rpg_sprite_set_hue(VALUE self, VALUE value) {
    RPGsprite *s = DATA_PTR(self);
    s->hue = NUM2FLT(value);
    while (s->hue >= 360.0f) {
        s->hue -= 360.0f;
    }
    while (s->hue < 0.0f) {
        s->hue += 360.0f;
    }
    return value;
}