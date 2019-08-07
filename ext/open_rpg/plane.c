#include "./plane.h"

VALUE rb_cPlane;

GLuint plane_sampler;

void rpg_plane_init(VALUE parent) {
    rb_cPlane = rb_define_class_under(parent, "Plane", rb_cRenderable);
    rb_define_alloc_func(rb_cPlane, rpg_plane_alloc);
    rb_define_method(rb_cPlane, "initialize", rpg_plane_initialize, -1);
    rb_define_method(rb_cPlane, "dispose", rpg_plane_dispose, -1);

    rb_define_method(rb_cPlane, "x", rpg_plane_get_x, 0);    
    rb_define_method(rb_cPlane, "y", rpg_plane_get_y, 0);
    rb_define_method(rb_cPlane, "width", rpg_plane_get_width, 0);
    rb_define_method(rb_cPlane, "height", rpg_plane_get_height, 0);
    rb_define_method(rb_cPlane, "x=", rpg_plane_set_x, 1);    
    rb_define_method(rb_cPlane, "y=", rpg_plane_set_y, 1);
    rb_define_method(rb_cPlane, "width=", rpg_plane_set_width, 1);
    rb_define_method(rb_cPlane, "height=", rpg_plane_set_height, 1);

    rb_define_method(rb_cPlane, "location", rpg_plane_get_location, 0);
    rb_define_method(rb_cPlane, "size", rpg_plane_get_size, 0);
    rb_define_method(rb_cPlane, "rect", rpg_plane_get_rect, 0);
    rb_define_method(rb_cPlane, "location=", rpg_plane_set_location, 1);
    rb_define_method(rb_cPlane, "size=", rpg_plane_set_size, 1);
    rb_define_method(rb_cPlane, "rect=", rpg_plane_set_rect, 1);

    rb_define_method(rb_cPlane, "zoom_x", rpg_plane_get_src_scale_x, 0);    
    rb_define_method(rb_cPlane, "zoom_y", rpg_plane_get_src_scale_y, 0);
    rb_define_method(rb_cPlane, "zoom_x=", rpg_plane_set_src_scale_x, 1);    
    rb_define_method(rb_cPlane, "zoom_y=", rpg_plane_set_src_scale_y, 1);
    rb_define_method(rb_cPlane, "ox=", rpg_plane_set_ox, 1);
    rb_define_method(rb_cPlane, "oy=", rpg_plane_set_oy, 1);

    rb_define_method(rb_cPlane, "image", rpg_plane_get_image, 0);
    rb_define_method(rb_cPlane, "image=", rpg_plane_set_image, 1);

    plane_sampler = 0;
}

ATTR_READER(rpg_plane_get_src_scale_x, RPGplane, src_scale.x, DBL2NUM)
ATTR_READER(rpg_plane_get_src_scale_y, RPGplane, src_scale.y, DBL2NUM)
ATTR_WRITER(rpg_plane_set_src_scale_x, RPGplane, src_scale.x, NUM2FLT)
ATTR_WRITER(rpg_plane_set_src_scale_y, RPGplane, src_scale.y, NUM2FLT)

static VALUE rpg_plane_alloc(VALUE klass) {
    RPGplane *p = ALLOC(RPGplane);
    memset(p, 0, sizeof(RPGplane));
    RPG_RENDER_INIT(p->base);
    p->update_vao = GL_TRUE;
    p->src_scale.x = 1.0f;
    p->src_scale.y = 1.0f;
    p->base.render = rpg_plane_render;
    return Data_Wrap_Struct(klass, NULL, RUBY_DEFAULT_FREE, p);
}

static VALUE rpg_plane_initialize(int argc, VALUE *argv, VALUE self) {
    VALUE viewport;
    rb_scan_args(argc, argv, "01", &viewport);

    RPGplane *p = DATA_PTR(self);
    if (RTEST(viewport)) {
        p->viewport = DATA_PTR(viewport);
        rpg_batch_add(p->viewport->batch, &p->base);
    } else {
        rpg_batch_add(game_batch, &p->base);
    }
    if (plane_sampler == 0) {
        glGenSamplers(1, &plane_sampler);
        glSamplerParameteri(plane_sampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glSamplerParameteri(plane_sampler, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glSamplerParameteri(plane_sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glSamplerParameteri(plane_sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }

    glGenVertexArrays(1, &p->vao);
    glBindVertexArray(p->vao);
    glGenBuffers(1, &p->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, p->vbo);
    glBufferData(GL_ARRAY_BUFFER, VERTICES_SIZE, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), NULL);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return Qnil;
}

static VALUE rpg_plane_dispose(int argc, VALUE *argv, VALUE self) {
    rb_call_super(0, NULL);
    RPGplane *p = DATA_PTR(self);
    if (p->vbo) {
        glDeleteBuffers(1, &p->vbo);
    }
    if (p->vao) {
        glDeleteVertexArrays(1, &p->vao);
    }
    return Qnil;
}

void rpg_plane_render(void *renderable) {
    RPGplane *p = renderable;
    if (p->base.visible && p->image && p->base.alpha > 0.0f) {

        if (p->update_vao) {

            GLfloat l = ((GLfloat) p->base.ox / p->image->width) * p->src_scale.x;
            GLfloat t = ((GLfloat) p->base.oy / p->image->height) * p->src_scale.y;
            GLfloat r = l + (((GLfloat) p->rect.width / p->image->width) * p->src_scale.x);
            GLfloat b = t + (((GLfloat) p->rect.height / p->image->height) * p->src_scale.y);

            glBindBuffer(GL_ARRAY_BUFFER, p->vbo);
            float vertices[VERTICES_COUNT] = 
            {
                0.0f, 1.0f, l, b,
                1.0f, 0.0f, r, t,
                0.0f, 0.0f, l, t, 
                0.0f, 1.0f, l, b,
                1.0f, 1.0f, r, b,
                1.0f, 0.0f, r, t
            };

            glBufferSubData(GL_ARRAY_BUFFER, 0, VERTICES_SIZE, vertices);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            p->update_vao = GL_FALSE;
        }

        if (p->base.updated) {
            GLfloat sx = p->base.scale.x * p->rect.width;
            GLfloat sy = p->base.scale.y * p->rect.height;
            GLfloat cos = cosf(p->base.rotation.radians);
            GLfloat sin = sinf(p->base.rotation.radians);
            MAT4_SET(p->base.model, 
                sx * cos, sx * sin,  0.0f,   0.0f, 
                sy * -sin, sy * cos, 0.0f,  0.0f, 
                0.0f, 0.0f, 1.0f, 0.0f,
                (p->base.rotation.ox * (1.0f - cos) + p->base.rotation.oy * sin) + p->rect.x,
                (p->base.rotation.oy * (1.0f - cos) - p->base.rotation.ox * sin) + p->rect.y, 0.0f, 1.0f
            );
            p->base.updated = GL_FALSE;
        }

        RPG_BASE_UNIFORMS(p);

        glBindSampler(0, plane_sampler);   
        RPG_RENDER_TEXTURE(p->image->texture, p->vao);
        glBindSampler(0, 0);
    }
}

ATTR_READER(rpg_plane_get_x, RPGplane, rect.x, INT2NUM)
ATTR_READER(rpg_plane_get_y, RPGplane, rect.y, INT2NUM)
ATTR_READER(rpg_plane_get_width, RPGplane, rect.width, INT2NUM)
ATTR_READER(rpg_plane_get_height, RPGplane, rect.height, INT2NUM)

static VALUE rpg_plane_set_x(VALUE self, VALUE value) {
    RPGplane *p = DATA_PTR(self);
    p->rect.x = NUM2INT(value);
    p->base.updated = GL_TRUE;
    return value;
}

static VALUE rpg_plane_set_y(VALUE self, VALUE value) {
    RPGplane *p = DATA_PTR(self);
    p->rect.y = NUM2INT(value);
    p->base.updated = GL_TRUE;
    return value;
}

static VALUE rpg_plane_set_width(VALUE self, VALUE value) {
    RPGplane *p = DATA_PTR(self);
    p->rect.width = NUM2INT(value);
    p->base.updated = GL_TRUE;
    return value;
}

static VALUE rpg_plane_set_height(VALUE self, VALUE value) {
    RPGplane *p = DATA_PTR(self);
    p->rect.height = NUM2INT(value);
    p->base.updated = GL_TRUE;
    return value;
}

static VALUE rpg_plane_get_location(VALUE self) {
    RPGplane *p = DATA_PTR(self);
    RPGpoint *pnt = ALLOC(RPGpoint);
    memcpy(pnt, &p->rect.x, sizeof(RPGpoint));
    return Data_Wrap_Struct(rb_cPoint, NULL, RUBY_DEFAULT_FREE, pnt);
}

static VALUE rpg_plane_get_size(VALUE self) {
    RPGplane *p = DATA_PTR(self);
    RPGsize *sz = ALLOC(RPGsize);
    memcpy(sz, &p->rect.width, sizeof(RPGsize));
    return Data_Wrap_Struct(rb_cSize, NULL, RUBY_DEFAULT_FREE, sz);
}

static VALUE rpg_plane_get_rect(VALUE self) {
    RPGplane *p = DATA_PTR(self);
    RPGrect *rect = ALLOC(RPGrect);
    memcpy(rect, &p->rect, sizeof(RPGrect));
    return Data_Wrap_Struct(rb_cRect, NULL, RUBY_DEFAULT_FREE, rect);
}

static VALUE rpg_plane_set_location(VALUE self, VALUE value) {
    RPGplane *p = DATA_PTR(self);
    RPGpoint *v = DATA_PTR(value);
    memcpy(&p->rect.x, v, sizeof(RPGpoint));
    p->base.updated = GL_TRUE;
    return value;
}

static VALUE rpg_plane_set_size(VALUE self, VALUE value) {
    RPGplane *p = DATA_PTR(self);
    RPGsize *v = DATA_PTR(value);
    memcpy(&p->rect.width, v, sizeof(RPGsize));
    p->base.updated = GL_TRUE;
    return value;
}

static VALUE rpg_plane_set_rect(VALUE self, VALUE value) {
    RPGplane *p = DATA_PTR(self);
    RPGrect *v = DATA_PTR(value);
    memcpy(&p->rect, v, sizeof(RPGrect));
    p->base.updated = GL_TRUE;
    return value;
}

static VALUE rpg_plane_get_image(VALUE self) {
    RPGplane *p = DATA_PTR(self);
    return p->image ? Data_Wrap_Struct(rb_cImage, NULL, NULL, p->image) : Qnil;
}

static VALUE rpg_plane_set_image(VALUE self, VALUE value) {
    RPGplane *p = DATA_PTR(self);
    p->image = RTEST(value) ? DATA_PTR(value) : NULL;
    return value;
}

static VALUE rpg_plane_set_ox(VALUE self, VALUE value) {
    RPGplane *p = DATA_PTR(self);
    p->update_vao = GL_TRUE;
    return rb_call_super(1, &value);
}

static VALUE rpg_plane_set_oy(VALUE self, VALUE value) {
    RPGplane *p = DATA_PTR(self);
    p->update_vao = GL_TRUE;
    return rb_call_super(1, &value);
}
