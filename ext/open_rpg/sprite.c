#include "./renderable.h"
#include "./internal.h"

VALUE rb_cSprite;

static VALUE rpg_sprite_dispose(int argc, VALUE *argv, VALUE self) {
    rb_call_super(0, NULL);
    RPGsprite *s = DATA_PTR(self);
    if (s->viewport != NULL) {
        rpg_batch_delete_item(s->viewport->batch, &s->base);
    } else {
        rpg_batch_delete_item(rpgRENDER_BATCH, &s->base);
    }
    VALUE img_dispose;
    rb_scan_args(argc, argv, "01", &img_dispose);
    if (RTEST(img_dispose) && s->image != NULL) {
        if (s->image->texture) {
            glDeleteTextures(1, &s->image->texture);
        }
        if (s->image->fbo) {
            glDeleteFramebuffers(1, &s->image->fbo);
        }
        RPG_FREE(s->image);
        s->image = NULL;
    }
    glDeleteVertexArrays(1, &s->vao);
    glDeleteBuffers(1, &s->vbo);
    return Qnil;
}

static VALUE rpg_sprite_alloc(VALUE klass) {
    RPGsprite *sprite = ALLOC(RPGsprite);
    memset(sprite, 0, sizeof(RPGsprite));
    RPG_RENDER_INIT(sprite->base);
    sprite->base.render = rpg_sprite_render;
    return Data_Wrap_Struct(klass, NULL, RUBY_DEFAULT_FREE, sprite);
}

static VALUE rpg_sprite_get_rect(VALUE self) {
    RPGsprite *sprite = DATA_PTR(self);
    RPGrect *rect = ALLOC(RPGrect);
    if (sprite->image) {
        memcpy(rect, &sprite->src_rect, sizeof(RPGrect));
    } else {
        memset(rect, 0, sizeof(RPGrect));
    }
    return Data_Wrap_Struct(rb_cRect, NULL, RUBY_DEFAULT_FREE, rect);
}

static inline void rpg_sprite_set_rect_inline(RPGsprite *s, int x, int y, int width, int height) {
    s->src_rect.x = x;
    s->src_rect.y = y;
    s->src_rect.width = width;
    s->src_rect.height = height;

    if (s->image == NULL) {
        return;
    }

    GLfloat l = (GLfloat) x / s->image->width;
    GLfloat t = (GLfloat) y / s->image->height;
    GLfloat r = l + ((GLfloat) width / s->image->width);
    GLfloat b = t + ((GLfloat) height / s->image->height);

    glBindBuffer(GL_ARRAY_BUFFER, s->vbo);
    float vertices[VERTICES_COUNT] = {0.0f, 1.0f, l, b, 1.0f, 0.0f, r, t, 0.0f, 0.0f, l, t,
                                      0.0f, 1.0f, l, b, 1.0f, 1.0f, r, b, 1.0f, 0.0f, r, t};
    glBufferSubData(GL_ARRAY_BUFFER, 0, VERTICES_SIZE, vertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    s->base.updated = GL_TRUE;
}

static VALUE rpg_sprite_set_rect(VALUE self, VALUE value) {
    RPGsprite *s = DATA_PTR(self);
    RPGrect *rect = DATA_PTR(value);
    rpg_sprite_set_rect_inline(s, rect->x, rect->y, rect->width, rect->height);
    return value;
}

static VALUE rpg_sprite_viewport(VALUE self) {
    RPGsprite *sprite = DATA_PTR(self);
    return (sprite->viewport) ? Data_Wrap_Struct(rb_cViewport, NULL, NULL, sprite->viewport) : Qnil;
}

void rpg_sprite_render(void *sprite) {
    RPGsprite *s = sprite;
    if (s->image && s->base.visible && s->base.alpha > __FLT_EPSILON__) {
        if (s->base.updated) {

            GLint x = s->x + s->base.ox;
            GLint y = s->y + s->base.oy;
            if (s->viewport != NULL) {
                x += s->viewport->base.ox;
                y += s->viewport->base.oy;
            }

            GLfloat sx = s->base.scale.x * s->src_rect.width;
            GLfloat sy = s->base.scale.y * s->src_rect.height;
            GLfloat cos = cosf(s->base.rotation.radians);
            GLfloat sin = sinf(s->base.rotation.radians);
            MAT4_SET(s->base.model, sx * cos, sx * sin, 0.0f, 0.0f, sy * -sin, sy * cos, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
                     (s->base.rotation.ox * (1.0f - cos) + s->base.rotation.oy * sin) + x,
                     (s->base.rotation.oy * (1.0f - cos) - s->base.rotation.ox * sin) + y, 0.0f, 1.0f);
            s->base.updated = GL_FALSE;
        }
        RPG_BASE_UNIFORMS(s);
        RPG_RENDER_TEXTURE(s->image->texture, s->vao);
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
        rpg_batch_add(rpgRENDER_BATCH, &sprite->base);
    }

    glGenVertexArrays(1, &sprite->vao);
    glBindVertexArray(sprite->vao);
    glGenBuffers(1, &sprite->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, sprite->vbo);
    glBufferData(GL_ARRAY_BUFFER, VERTICES_SIZE, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), NULL);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    if (RTEST(options)) {
        VALUE opt = rb_hash_aref(options, STR2SYM("image"));
        if (RTEST(opt)) {
            sprite->image = DATA_PTR(opt);
            if (sprite->image != NULL) {
                rpg_sprite_set_rect_inline(sprite, 0, 0, sprite->image->width, sprite->image->height);
            }
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
            rpgRENDER_BATCH->updated = GL_TRUE;
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
    return sprite->image ? Data_Wrap_Struct(rb_cImage, NULL, NULL, sprite->image) : Qnil;
}

static VALUE rpg_sprite_set_image(VALUE self, VALUE value) {
    RPGsprite *sprite = DATA_PTR(self);
    sprite->image = RTEST(value) ? DATA_PTR(value) : NULL;
    if (sprite->image != NULL) {
        rpg_sprite_set_rect_inline(sprite, 0, 0, sprite->image->width, sprite->image->height);
    } else {
        rpg_sprite_set_rect_inline(sprite, 0, 0, 0, 0);
    }
    return value;
}

void rpg_sprite_init(VALUE parent) {
    // Sprite
    rb_cSprite = rb_define_class_under(parent, "Sprite", rb_cRenderable);
    rb_define_alloc_func(rb_cSprite, rpg_sprite_alloc);
    rb_define_method(rb_cSprite, "initialize", rpg_sprite_initialize, -1);
    rb_define_method(rb_cSprite, "dispose", rpg_sprite_dispose, -1);

    rb_define_method(rb_cSprite, "viewport", rpg_sprite_viewport, 0);
    rb_define_method(rb_cSprite, "x", rpg_sprite_get_x, 0);
    rb_define_method(rb_cSprite, "x=", rpg_sprite_set_x, 1);
    rb_define_method(rb_cSprite, "y", rpg_sprite_get_y, 0);
    rb_define_method(rb_cSprite, "y=", rpg_sprite_set_y, 1);
    rb_define_method(rb_cSprite, "z=", rpg_sprite_set_z, 1);
    rb_define_method(rb_cSprite, "image", rpg_sprite_get_image, 0);
    rb_define_method(rb_cSprite, "image=", rpg_sprite_set_image, 1);
    rb_define_method(rb_cSprite, "src_rect", rpg_sprite_get_rect, 0);
    rb_define_method(rb_cSprite, "src_rect=", rpg_sprite_set_rect, 1);
}