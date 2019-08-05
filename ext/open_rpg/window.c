#include "./window.h"

VALUE rb_cWindow;

void rpg_window_init(VALUE parent) {
    rb_cWindow = rb_define_class_under(parent, "Window", rb_cViewport);
    rb_define_alloc_func(rb_cWindow, rpg_viewport_alloc);

    rb_define_method(rb_cWindow, "x", rpg_window_get_x, 0);
    rb_define_method(rb_cWindow, "x=", rpg_window_set_x, 1);

    rb_define_method(rb_cWindow, "y", rpg_window_get_y, 0);
    rb_define_method(rb_cWindow, "y=", rpg_window_set_y, 1);

    rb_define_method(rb_cWindow, "move", rpg_window_move, -1);
    rb_define_method(rb_cWindow, "width", rpg_window_width, 0);
    rb_define_method(rb_cWindow, "height", rpg_window_height, 0);
    // rb_define_method(rb_cWindow, "size", rpg_window_size, 0);
    // rb_define_method(rb_cWindow, "location", rpg_window_location, 0);
    // rb_define_method(rb_cWindow, "rect", rpg_window_rect, 0);
}

// static VALUE rpg_window_alloc(VALUE klass) {
//     RPGwindow *w = ALLOC(RPGwindow);
//     memset(w, 0, sizeof(RPGwindow));
//     // TODO: set base

//     return Data_Wrap_Struct(rb_cWindow, NULL, RUBY_DEFAULT_FREE, w);
// }

// static VALUE rpg_window_initialize(VALUE self, VALUE x, VALUE y, VALUE width, VALUE height) {
//     RPGwindow *w = DATA_PTR(self);
//     w->rect.x = NUM2INT(x);
//     w->rect.y = NUM2INT(y);
//     w->rect.width = NUM2INT(width);
//     w->rect.height = NUM2INT(height);
//     check_dimensions(w->rect.width, w->rect.height);
//     return Qnil;
// }

ATTR_READER(rpg_window_get_x, RPGwindow, rect.x, INT2NUM)
ATTR_READER(rpg_window_get_y, RPGwindow, rect.y, INT2NUM)
ATTR_READER(rpg_window_width, RPGwindow, rect.width, INT2NUM)
ATTR_READER(rpg_window_height, RPGwindow, rect.height, INT2NUM)

static VALUE rpg_window_set_x(VALUE self, VALUE value) {
    RPGwindow *w = DATA_PTR(self);
    int x = NUM2INT(value);
    if (w->rect.x != x) {
        w->rect.x = x;
        w->base.updated = GL_TRUE;
    }
    return value;
}

static VALUE rpg_window_set_y(VALUE self, VALUE value) {
    RPGwindow *w = DATA_PTR(self);
    int y = NUM2INT(value);
    if (w->rect.y != y) {
        w->rect.y = y;
        w->base.updated = GL_TRUE;
    }
    return value;
}

// static VALUE rpg_window_size(VALUE self) {
//     RPGwindow *w = DATA_PTR(self);
//     RPGsize *size = ALLOC(RPGsize);
//     memcpy(size, &w->rect.width, sizeof(RPGsize));
//     return Data_Wrap_Struct(rb_cSize, NULL, RUBY_DEFAULT_FREE, size);
// }

// static VALUE rpg_window_location(VALUE self) {
//     RPGwindow *w = DATA_PTR(self);
//     RPGpoint *pnt = ALLOC(RPGpoint);
//     memcpy(pnt, &w->rect.x, sizeof(RPGpoint));
//     return Data_Wrap_Struct(rb_cPoint, NULL, RUBY_DEFAULT_FREE, pnt);
// }

// static VALUE rpg_window_rect(VALUE self) {
//     RPGwindow *w = DATA_PTR(self);
//     RPGrect *rect = ALLOC(RPGrect);
//     memcpy(rect, &w->rect, sizeof(RPGrect));
//     return Data_Wrap_Struct(rb_cRect, NULL, RUBY_DEFAULT_FREE, rect);
// }

static inline void rpg_window_move_inline(RPGwindow *w, int x, int y) {
    if (w->rect.x != x || w->rect.y != y) {
        w->rect.x = x;
        w->rect.y = y;
        w->base.updated = GL_TRUE;
    }
}

static VALUE rpg_window_move(int argc, VALUE *argv, VALUE self) {
    VALUE a1, a2;
    rb_scan_args(argc, argv, "11", &a1, &a2);
    RPGwindow *w = DATA_PTR(self);
    if (argc == 1) {
        RPGpoint *pnt = DATA_PTR(a1);
        rpg_window_move_inline(w, pnt->x, pnt->y);
    } else {
        rpg_window_move_inline(w, NUM2INT(a1), NUM2INT(a2));
    }
    return self;
}