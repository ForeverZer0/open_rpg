#include "./renderable.h"

VALUE rb_cRenderable;

void rpg_renderable_init(VALUE parent) {
    rb_cRenderable = rb_define_class_under(parent, "Renderable", rb_cObject);

    rb_define_alloc_func(rb_cRenderable, rpg_renderable_alloc);
    rb_define_method(rb_cRenderable, "update", rpg_renderable_update, 0);

    rb_define_method(rb_cRenderable, "angle", rpg_renderable_get_angle, 0);
    rb_define_method(rb_cRenderable, "angle=", rpg_renderable_set_angle, 1);
    rb_define_method(rb_cRenderable, "anchor", rpg_renderable_get_anchor, 0);
    rb_define_method(rb_cRenderable, "anchor=", rpg_renderable_set_anchor, 1);
    
    rb_define_method(rb_cRenderable, "alpha", rpg_renderable_get_alpha, 0);
    rb_define_method(rb_cRenderable, "alpha=", rpg_renderable_set_alpha, 1);
    rb_define_method(rb_cRenderable, "opacity", rpg_renderable_get_opacity, 0);
    rb_define_method(rb_cRenderable, "opacity=", rpg_renderable_set_opacity, 1);
    rb_define_method(rb_cRenderable, "visible", rpg_renderable_get_visible, 0);
    rb_define_method(rb_cRenderable, "visible=", rpg_renderable_set_visible, 1);

    rb_define_method(rb_cRenderable, "origin", rpg_renderable_get_origin, 0);
    rb_define_method(rb_cRenderable, "origin=", rpg_renderable_set_origin, 1);
    rb_define_method(rb_cRenderable, "ox", rpg_renderable_get_ox, 0);
    rb_define_method(rb_cRenderable, "ox=", rpg_renderable_set_ox, 1);
    rb_define_method(rb_cRenderable, "oy", rpg_renderable_get_oy, 0);
    rb_define_method(rb_cRenderable, "oy=", rpg_renderable_set_oy, 1);
    rb_define_method(rb_cRenderable, "z", rpg_renderable_get_z, 0);
    rb_define_method(rb_cRenderable, "z=", rpg_renderable_set_z, 1);

    rb_define_method(rb_cRenderable, "scale", rpg_renderable_get_scale, 0);
    rb_define_method(rb_cRenderable, "scale=", rpg_renderable_set_scale, 1);
    rb_define_method(rb_cRenderable, "zoom", rpg_renderable_zoom, -1);
    rb_define_method(rb_cRenderable, "scale_x", rpg_renderable_get_scale_x, 0);
    rb_define_method(rb_cRenderable, "scale_x=", rpg_renderable_set_scale_x, 1);
    rb_define_method(rb_cRenderable, "scale_y", rpg_renderable_get_scale_y, 0);
    rb_define_method(rb_cRenderable, "scale_y=", rpg_renderable_set_scale_y, 1);

    rb_define_method(rb_cRenderable, "blend", rpg_renderable_get_blend, 0);
    rb_define_method(rb_cRenderable, "blend=", rpg_renderable_set_blend, 1);

    rb_define_method(rb_cRenderable, "color", rpg_renderable_get_color, 0);
    rb_define_method(rb_cRenderable, "color=", rpg_renderable_set_color, 1);
    rb_define_method(rb_cRenderable, "tone", rpg_renderable_get_tone, 0);
    rb_define_method(rb_cRenderable, "tone=", rpg_renderable_set_tone, 1);
    rb_define_method(rb_cRenderable, "hue", rpg_renderable_get_hue, 0);
    rb_define_method(rb_cRenderable, "hue=", rpg_renderable_set_hue, 1);
    
    rb_define_method(rb_cRenderable, "flash", rpg_renderable_flash, 2);
    rb_define_method(rb_cRenderable, "flash_color", rpg_renderable_flash_color, 0);
    rb_define_method(rb_cRenderable, "flash_duration", rpg_renderable_flash_duration, 0);
    rb_define_method(rb_cRenderable, "flashing?", rpg_renderable_flashing_p, 0);

    rb_define_method(rb_cRenderable, "dispose", rpg_renderable_dispose, 0);
    rb_define_method(rb_cRenderable, "disposed?", rpg_renderable_disposed_p, 0);
}

VALUE rpg_renderable_alloc(VALUE klass) {
    RPGrenderable *r = ALLOC(RPGrenderable);
    memset(r, 0, sizeof(RPGrenderable));
    r->scale.x = 1.0f;                                                                                                                      
    r->scale.y = 1.0f;                                                                                                                      
    r->blend.equation = GL_FUNC_ADD;                                                                                                        
    r->blend.src_factor = GL_SRC_ALPHA;                                                                                                     
    r->blend.dst_factor = GL_ONE_MINUS_SRC_ALPHA;                                                                                           
    r->visible = GL_TRUE;                                                                                                                   
    r->alpha = 1.0f;       
    r->hue = 0.0f;                                                                                                                 
    r->updated = GL_TRUE;
    return Data_Wrap_Struct(klass, NULL, RUBY_DEFAULT_FREE, r);
}

ATTR_READER(rpg_renderable_get_alpha, RPGrenderable, alpha, DBL2NUM)
ATTR_READER(rpg_renderable_get_ox, RPGrenderable, ox, INT2NUM)
ATTR_READER(rpg_renderable_get_oy, RPGrenderable, oy, INT2NUM)
ATTR_READER(rpg_renderable_get_z, RPGrenderable, z, INT2NUM)

ATTR_WRITER(rpg_renderable_set_ox, RPGrenderable, ox, NUM2INT) // TODO: Update model
ATTR_WRITER(rpg_renderable_set_oy, RPGrenderable, oy, NUM2INT)
ATTR_WRITER(rpg_renderable_set_z, RPGrenderable, z, NUM2INT)

static VALUE rpg_renderable_dispose(VALUE self) {
    RPGrenderable *r = DATA_PTR(self);
    r->disposed = GL_TRUE;
    return Qnil;
}

static VALUE rpg_renderable_disposed_p(VALUE self) {
    RPGrenderable *r = DATA_PTR(self);
    return RB_BOOL(r->disposed);
}

static VALUE rpg_renderable_update(VALUE self) {
    RPGrenderable *r = DATA_PTR(self);
    if (r->flash.duration > 0) {
        r->flash.duration--;
        if (r->flash.duration == 0) {
            memset(&r->flash.color, 0, sizeof(RPGcolor));
        }
    }
    return self;
}

static VALUE rpg_renderable_get_origin(VALUE self) {
    RPGrenderable *r = DATA_PTR(self);
    RPGpoint *point = ALLOC(RPGpoint);
    point->x = r->ox;
    point->y = r->oy;
    return Data_Wrap_Struct(rb_cPoint, NULL, RUBY_DEFAULT_FREE, point);
}

static VALUE rpg_renderable_set_origin(VALUE self, VALUE value) {
    RPGrenderable *r = DATA_PTR(self);
    RPGpoint *point = DATA_PTR(value);
    r->ox = point->x;
    r->oy = point->y;
    return value;
}

static VALUE rpg_renderable_get_visible(VALUE self) {
    RPGrenderable *r = DATA_PTR(self);
    return RB_BOOL(r->visible);
}

static VALUE rpg_renderable_set_visible(VALUE self, VALUE value) {
    RPGrenderable *r = DATA_PTR(self);
    r->visible = RTEST(value);
    return value;
} 

static VALUE rpg_renderable_get_color(VALUE self) {
    RPGrenderable *r = DATA_PTR(self);
    return Data_Wrap_Struct(rb_cColor, NULL, NULL, &r->color);
}

static VALUE rpg_renderable_set_color(VALUE self, VALUE value) {
    RPGrenderable *r = DATA_PTR(self);
    RPGcolor *color = DATA_PTR(value);
    memcpy(&r->color, color, sizeof(RPGcolor));
    return value;
}

static VALUE rpg_renderable_get_tone(VALUE self) {
    RPGrenderable *r = DATA_PTR(self);
    return Data_Wrap_Struct(rb_cTone, NULL, NULL, &r->tone);
}

static VALUE rpg_renderable_set_tone(VALUE self, VALUE value) {
    RPGrenderable *r = DATA_PTR(self);
    RPGtone *tone = DATA_PTR(value);
    memcpy(&r->tone, tone, sizeof(RPGtone));
    return value;
}

static VALUE rpg_renderable_flash(VALUE self, VALUE color, VALUE duration) {
    RPGrenderable *r = DATA_PTR(self);
    if (NIL_P(color)) {
        memset(&r->flash, 0, sizeof(RPGflash));
    } else {
        RPGcolor *c = DATA_PTR(color);
        memcpy(&r->flash.color, c, sizeof(RPGcolor));
        r->flash.duration = (GLubyte)NUM2CHR(duration);
    }
    return self;
}

static VALUE rpg_renderable_flash_duration(VALUE self) {
    RPGrenderable *r = DATA_PTR(self);
    return INT2NUM(r->flash.duration);
}

static VALUE rpg_renderable_flash_color(VALUE self) {
    RPGrenderable *r = DATA_PTR(self);
    return Data_Wrap_Struct(rb_cColor, NULL, NULL, &r->flash.color);
}

static VALUE rpg_renderable_flashing_p(VALUE self) {
    RPGrenderable *r = DATA_PTR(self);
    return RB_BOOL(r->flash.duration > 0);
}

static VALUE rpg_renderable_set_alpha(VALUE self, VALUE value) {
    RPGrenderable *r = DATA_PTR(self);
    r->alpha = clampf(NUM2FLT(value), 0.0f, 1.0f);
    return value;
}

static VALUE rpg_renderable_get_opacity(VALUE self) {
    RPGrenderable *r = DATA_PTR(self);
    return INT2NUM((int)roundf(r->alpha * 255.0f));
}

static VALUE rpg_renderable_set_opacity(VALUE self, VALUE value) {
    RPGrenderable *r = DATA_PTR(self);
    r->alpha = clampf(NUM2INT(value) / 255.0f, 0.0f, 1.0f);
    return value;
}

static VALUE rpg_renderable_get_angle(VALUE self) {
    RPGrenderable *renderable = DATA_PTR(self);
    const float f = 180.0f / FLT_PI;
    return DBL2NUM((f * renderable->rotation.radians));
}

static VALUE rpg_renderable_set_angle(VALUE self, VALUE value) {
    RPGrenderable *renderable = DATA_PTR(self);
    const float f = FLT_PI / 180.0f;
    renderable->rotation.radians = f * clampf(NUM2FLT(value), 0.0f, 360.0f);
    renderable->updated = GL_TRUE;
    return value;
}

static VALUE rpg_renderable_get_anchor(VALUE self) {
    RPGrenderable *renderable = DATA_PTR(self);
    RPGpoint *point = ALLOC(RPGpoint);
    point->x = renderable->rotation.ox;
    point->y = renderable->rotation.oy;
    return Data_Wrap_Struct(rb_cPoint, NULL, RUBY_DEFAULT_FREE, point);
}

static VALUE rpg_renderable_set_anchor(VALUE self, VALUE value) {
    RPGrenderable *renderable = DATA_PTR(self);
    RPGpoint *point = DATA_PTR(value);
    renderable->rotation.ox = point->x;
    renderable->rotation.oy = point->y;
    renderable->updated = GL_TRUE;
    return value;
}

static VALUE rpg_renderable_get_scale(VALUE self) {
    RPGrenderable *renderable = DATA_PTR(self);
    RPGvector2 *vec = ALLOC(RPGvector2);
    memcpy(vec, &renderable->scale, sizeof(RPGvector2));
    return Data_Wrap_Struct(rb_cVector2, NULL, RUBY_DEFAULT_FREE, vec);
}

static VALUE rpg_renderable_set_scale(VALUE self, VALUE value) {
    RPGrenderable *renderable = DATA_PTR(self);
    RPGvector2 *vec = DATA_PTR(value);
    renderable->scale.x = vec->x;
    renderable->scale.y = vec->y;
    renderable->updated = GL_TRUE;
    return value;
}

static VALUE rpg_renderable_zoom(int argc, VALUE *argv, VALUE self) {
    VALUE a1, a2;
    rb_scan_args(argc, argv, "11", &a1, &a2);
    RPGrenderable *renderable = DATA_PTR(self);
    renderable->scale.x = NUM2FLT(a1);
    if (argc == 1) {
        renderable->scale.y = renderable->scale.x;
    } else {
        renderable->scale.y = NUM2FLT(a2);
    }
    renderable->updated = GL_TRUE;
    return self;
}

static VALUE rpg_renderable_get_scale_x(VALUE self) {
    RPGrenderable *renderable = DATA_PTR(self);
    return DBL2NUM(renderable->scale.x);
}

static VALUE rpg_renderable_set_scale_x(VALUE self, VALUE value) {
    RPGrenderable *renderable = DATA_PTR(self);
    renderable->scale.x = NUM2FLT(value);
    renderable->updated = GL_TRUE;
    return value;
}

static VALUE rpg_renderable_get_scale_y(VALUE self) {
    RPGrenderable *renderable = DATA_PTR(self);
    return DBL2NUM(renderable->scale.y);
}

static VALUE rpg_renderable_set_scale_y(VALUE self, VALUE value) {
    RPGrenderable *renderable = DATA_PTR(self);
    renderable->scale.y = NUM2FLT(value);
    renderable->updated = GL_TRUE;
    return value;
}

static VALUE rpg_renderable_get_blend(VALUE self) {
    RPGrenderable *renderable = DATA_PTR(self);
    RPGblend *blend = ALLOC(RPGblend);
    memcpy(blend, &renderable->blend, sizeof(RPGblend));
    return Data_Wrap_Struct(rb_cBlend, NULL, RUBY_DEFAULT_FREE, blend);
}

static VALUE rpg_renderable_set_blend(VALUE self, VALUE value) {
    RPGrenderable *renderable = DATA_PTR(self);
    if (NIL_P(value)) {
        rb_raise(rb_eArgError, "value cannot be nil");
    }
    RPGblend *blend = DATA_PTR(value);
    memcpy(&renderable->blend, blend, sizeof(RPGblend));
    return value;
}

static VALUE rpg_renderable_get_hue(VALUE self) {
    RPGrenderable *s = DATA_PTR(self);
    return DBL2NUM(s->hue);
}

static VALUE rpg_renderable_set_hue(VALUE self, VALUE value) {
    RPGrenderable *s = DATA_PTR(self);
    s->hue = NUM2FLT(value);
    while (s->hue >= 360.0f) {
        s->hue -= 360.0f;
    }
    while (s->hue < 0.0f) {
        s->hue += 360.0f;
    }
    return value;
}