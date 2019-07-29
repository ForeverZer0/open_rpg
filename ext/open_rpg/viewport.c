#include "viewport.h"

VALUE rb_cViewport;

void rpg_viewport_init(VALUE parent) {
    rb_cViewport = rb_define_class_under(parent, "Viewport", rb_cRenderable);
    rb_define_alloc_func(rb_cViewport, rpg_viewport_alloc);
    rb_define_method(rb_cViewport, "initialize", rpg_viewport_initialize, -1);

    rb_define_method(rb_cViewport, "rect", rpg_viewport_rect, 0);
    rb_define_method(rb_cViewport, "location", rpg_viewport_location, 0);
    rb_define_method(rb_cViewport, "size", rpg_viewport_size, 0);


}

ALLOC_FUNC(rpg_viewport_alloc, RPGviewport)

static VALUE rpg_viewport_initialize(int argc, VALUE *argv, VALUE self) {
    VALUE a1, a2, a3, a4;
    rb_scan_args(argc, argv, "04", &a1, &a2, &a3, &a4);
    rb_call_super(0, NULL);
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
    return Qnil;
}

static VALUE rpg_viewport_dispose(VALUE self) {

    return Qnil;
}

static VALUE rpg_viewport_disposed_p(VALUE self) {

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