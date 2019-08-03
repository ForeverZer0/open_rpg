#include "./window.h"

VALUE rb_cWindow;

void rpg_window_init(VALUE parent) {
    rb_cWindow = rb_define_class_under(parent, "Window", rb_cRenderable);
    rb_define_alloc_func(rb_cWindow, rpg_window_alloc);
    rb_define_method(rb_cWindow, "initialize", rpg_window_initialize, 4);

    rb_define_method(rb_cWindow, "x", rpg_window_get_x, 0);
    rb_define_method(rb_cWindow, "y", rpg_window_get_y, 0);
    rb_define_method(rb_cWindow, "width", rpg_window_get_width, 0);
    rb_define_method(rb_cWindow, "height", rpg_window_get_height, 0);
    rb_define_method(rb_cWindow, "size", rpg_window_get_size, 0);
    rb_define_method(rb_cWindow, "location", rpg_window_get_location, 0);
    rb_define_method(rb_cWindow, "rect", rpg_window_get_rect, 0);
    rb_define_method(rb_cWindow, "x=", rpg_window_set_x, 1);
    rb_define_method(rb_cWindow, "y=", rpg_window_set_y, 1);
    rb_define_method(rb_cWindow, "width=", rpg_window_set_width, 1);
    rb_define_method(rb_cWindow, "height=", rpg_window_set_height, 1);
    rb_define_method(rb_cWindow, "size=", rpg_window_set_size, 1);
    rb_define_method(rb_cWindow, "location=", rpg_window_set_location, 1);
    rb_define_method(rb_cWindow, "rect=", rpg_window_set_rect, 1);
    rb_define_method(rb_cWindow, "move", rpg_window_move, 4);

    rb_define_method(rb_cWindow, "windowskin", rpg_window_get_windowskin, 0);
    rb_define_method(rb_cWindow, "windowskin=", rpg_window_set_windowskin, 1);
    rb_define_method(rb_cWindow, "dispose", rpg_window_dispose, 0);
}

static VALUE rpg_window_alloc(VALUE klass) {
    RPGwindow *w = ALLOC(RPGwindow);
    memset(w, 0, sizeof(RPGwindow));
    // TODO: set base

    return Data_Wrap_Struct(rb_cWindow, NULL, RUBY_DEFAULT_FREE, w);
}

static VALUE rpg_window_dispose(VALUE self) {
    rb_call_super(0, NULL);
    RPGwindow *w = ALLOC(RPGwindow);
    if (w->texture) {
        glDeleteTextures(1, &w->texture);
    }
    if (w->fbo) {
        glDeleteFramebuffers(1, &w->fbo);
    }
    return Qnil;
}

static VALUE rpg_window_initialize(VALUE self, VALUE x, VALUE y, VALUE width, VALUE height) {
    RPGwindow *w = DATA_PTR(self);
    w->rect.x = NUM2INT(x);
    w->rect.y = NUM2INT(y);
    w->rect.width = NUM2INT(width);
    w->rect.height = NUM2INT(height);
    check_dimensions(w->rect.width, w->rect.height);
    return Qnil;
}

ATTR_READER(rpg_window_get_x, RPGwindow, rect.x, INT2NUM)
ATTR_READER(rpg_window_get_y, RPGwindow, rect.y, INT2NUM)
ATTR_READER(rpg_window_get_width, RPGwindow, rect.width, INT2NUM)
ATTR_READER(rpg_window_get_height, RPGwindow, rect.height, INT2NUM)

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

static VALUE rpg_window_set_width(VALUE self, VALUE value) {
    RPGwindow *w = DATA_PTR(self);
    int width = NUM2INT(value);
    if (w->rect.width != width) {
        if (width < 1) {
            rb_raise(rb_eArgError, "width must be greater than 0");
        }
        w->rect.width = width;
        w->invalidated = GL_TRUE;
        w->base.updated = GL_TRUE;
    }
    return value;
}

static VALUE rpg_window_set_height(VALUE self, VALUE value) {
    RPGwindow *w = DATA_PTR(self);
    int height = NUM2INT(value);
    if (w->rect.height != height) {
        if (height < 1) {
            rb_raise(rb_eArgError, "height must be greater than 0");
        }
        w->rect.height = height;
        w->invalidated = GL_TRUE;
        w->base.updated = GL_TRUE;
    }
    return value;
}

static VALUE rpg_window_get_size(VALUE self) {
    RPGwindow *w = DATA_PTR(self);
    RPGsize *size = ALLOC(RPGsize);
    memcpy(size, &w->rect.width, sizeof(RPGsize));
    return Data_Wrap_Struct(rb_cSize, NULL, RUBY_DEFAULT_FREE, size);
}

static VALUE rpg_window_get_location(VALUE self) {
    RPGwindow *w = DATA_PTR(self);
    RPGpoint *pnt = ALLOC(RPGpoint);
    memcpy(pnt, &w->rect.x, sizeof(RPGpoint));
    return Data_Wrap_Struct(rb_cPoint, NULL, RUBY_DEFAULT_FREE, pnt);
}

static VALUE rpg_window_get_rect(VALUE self) {
    RPGwindow *w = DATA_PTR(self);
    RPGrect *rect = ALLOC(RPGrect);
    memcpy(rect, &w->rect, sizeof(RPGrect));
    return Data_Wrap_Struct(rb_cRect, NULL, RUBY_DEFAULT_FREE, rect);
}

static VALUE rpg_window_set_size(VALUE self,  VALUE value) {
    RPGwindow *w = DATA_PTR(self);
    RPGsize *size = DATA_PTR(value);
    check_dimensions(size->width, size->height);
    memcpy(&w->rect.width, size, sizeof(RPGsize));
    w->invalidated = GL_TRUE;
    w->base.updated = GL_TRUE;
    return value;
}

static VALUE rpg_window_set_location(VALUE self,  VALUE value) {
    RPGwindow *w = DATA_PTR(self);
    RPGpoint *point = DATA_PTR(value);
    memcpy(&w->rect.x, point, sizeof(RPGpoint));
    w->base.updated = GL_TRUE;
    return value;
}

static VALUE rpg_window_set_rect(VALUE self,  VALUE value) {
    RPGwindow *w = DATA_PTR(self);
    RPGrect *rect = DATA_PTR(value);
    check_dimensions(rect->width, rect->height);
    memcpy(&w->rect, rect, sizeof(RPGrect));
    w->invalidated = GL_TRUE;
    w->base.updated = GL_TRUE;
    return value;
}

static VALUE rpg_window_move(VALUE self, VALUE x, VALUE y, VALUE width, VALUE height) {
    RPGwindow *w = DATA_PTR(self);
    w->rect.x = NUM2INT(x);
    w->rect.y = NUM2INT(y);
    w->rect.width = NUM2INT(width);
    w->rect.height = NUM2INT(height);
    check_dimensions(w->rect.width, w->rect.height);
    w->invalidated = GL_TRUE;
    w->base.updated = GL_TRUE;
    return self;
}

static VALUE rpg_window_get_windowskin(VALUE self) {
    RPGwindow *w = DATA_PTR(self);
    return w->windowskin ? Data_Wrap_Struct(rb_cImage, NULL, RUBY_DEFAULT_FREE, w->windowskin) : Qnil;
}

static VALUE rpg_window_set_windowskin(VALUE self, VALUE value) {
    RPGwindow *w = DATA_PTR(self);
    w->invalidated = GL_TRUE;
    w->windowskin = NIL_P(value) ? NULL : DATA_PTR(value);
    return value;
}