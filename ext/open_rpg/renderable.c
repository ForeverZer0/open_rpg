#include "renderable.h"

VALUE rb_cRenderable;

void rpg_renderable_init(VALUE parent) {
    rb_cRenderable = rb_define_class_under(parent, "Renderable", rb_cObject);
    rb_define_alloc_func(rb_cRenderable, rpg_renderable_alloc);
    rb_define_method(rb_cRenderable, "update", rpg_renderable_update, 0);

    rb_define_method(rb_cRenderable, "alpha", rpg_renderable_get_alpha, 0);
    rb_define_method(rb_cRenderable, "alpha=", rpg_renderable_set_alpha, 1);
    rb_define_method(rb_cRenderable, "opacity", rpg_renderable_get_opacity, 0);
    rb_define_method(rb_cRenderable, "opacity=", rpg_renderable_set_opacity, 1);
    rb_define_method(rb_cRenderable, "color", rpg_renderable_get_color, 0);
    rb_define_method(rb_cRenderable, "color=", rpg_renderable_set_color, 1);
    rb_define_method(rb_cRenderable, "tone", rpg_renderable_get_tone, 0);
    rb_define_method(rb_cRenderable, "tone=", rpg_renderable_set_tone, 1);
    rb_define_method(rb_cRenderable, "origin", rpg_renderable_get_origin, 0);
    rb_define_method(rb_cRenderable, "origin=", rpg_renderable_set_origin, 1);
    rb_define_method(rb_cRenderable, "ox", rpg_renderable_get_ox, 0);
    rb_define_method(rb_cRenderable, "ox=", rpg_renderable_set_ox, 1);
    rb_define_method(rb_cRenderable, "oy", rpg_renderable_get_oy, 0);
    rb_define_method(rb_cRenderable, "oy=", rpg_renderable_set_oy, 1);
    rb_define_method(rb_cRenderable, "z", rpg_renderable_get_z, 0);
    rb_define_method(rb_cRenderable, "z=", rpg_renderable_set_z, 1);
    rb_define_method(rb_cRenderable, "visible", rpg_renderable_get_visible, 0);
    rb_define_method(rb_cRenderable, "visible=", rpg_renderable_set_visible, 1);

    rb_define_method(rb_cRenderable, "flash", rpg_renderable_flash, 2);
    rb_define_method(rb_cRenderable, "flash_color", rpg_renderable_flash_color, 0);
    rb_define_method(rb_cRenderable, "flash_duration", rpg_renderable_flash_duration, 0);
    rb_define_method(rb_cRenderable, "flashing?", rpg_renderable_flashing_p, 0);
    
    rb_include_module(rb_cRenderable, rb_mDisposable);
    rb_define_method(rb_cRenderable, "dispose", rpg_renderable_dispose, 0);
    rb_define_method(rb_cRenderable, "disposed?", rpg_renderable_disposed_p, 0);
}

ALLOC_FUNC(rpg_renderable_alloc, RPGrenderable)

ATTR_READER(rpg_renderable_get_alpha, RPGrenderable, alpha, DBL2NUM)
ATTR_READER(rpg_renderable_get_ox, RPGrenderable, ox, INT2NUM)
ATTR_READER(rpg_renderable_get_oy, RPGrenderable, oy, INT2NUM)
ATTR_READER(rpg_renderable_get_z, RPGrenderable, z, INT2NUM)

ATTR_WRITER(rpg_renderable_set_ox, RPGrenderable, ox, NUM2INT) // TODO: Update model
ATTR_WRITER(rpg_renderable_set_oy, RPGrenderable, oy, NUM2INT)
ATTR_WRITER(rpg_renderable_set_z, RPGrenderable, z, NUM2INT)

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

static VALUE rpg_renderable_dispose(VALUE self) {
    rb_raise(rb_eNotImpError, "\"dispose\" has not been implemented");
    return Qnil;
}

static VALUE rpg_renderable_disposed_p(VALUE self) {
    rb_raise(rb_eNotImpError, "\"disposed?\" has not been implemented");
    return Qnil;
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
    r->alpha = clampf(NUM2INT(self) / 255.0f, 0.0f, 1.0f);
    return value;
}