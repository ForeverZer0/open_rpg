#include "./windowskin.h"

VALUE rb_cWindowskin;

#define CURSOR_COUNT 4

#define GET_RECT_FUNC(function, field) \
static VALUE function(VALUE self) { \
    RPGwindowskin *w = DATA_PTR(self); \
    RPGrect *r = ALLOC(RPGrect); \
    memcpy(r, &w->field, sizeof(RPGrect)); \
    return Data_Wrap_Struct(rb_cRect, NULL, RUBY_DEFAULT_FREE, r); \
}

#define SET_RECT_FUNC(function, field) \
static VALUE function(VALUE self, VALUE value) {\
    RPGwindowskin *w = DATA_PTR(self);\
    RPGrect *r = DATA_PTR(value);\
    memcpy(&w->field, r, sizeof(RPGrect));\
    return value;\
}

void rpg_windowskin_init(VALUE parent) {
    rb_cWindowskin = rb_define_class_under(parent, "Windowskin", rb_cObject);
    rb_define_alloc_func(rb_cWindowskin, rpg_windowskin_alloc);

    rb_define_method(rb_cWindowskin, "frame", rpg_windowskin_get_frame, 0);
    rb_define_method(rb_cWindowskin, "frame=", rpg_windowskin_set_frame, 1);
    rb_define_method(rb_cWindowskin, "background", rpg_windowskin_get_background, 0);
    rb_define_method(rb_cWindowskin, "background=", rpg_windowskin_set_background, 1);
    rb_define_method(rb_cWindowskin, "overlay", rpg_windowskin_get_overlay, 0);
    rb_define_method(rb_cWindowskin, "overlay=", rpg_windowskin_set_overlay, 1);
    rb_define_method(rb_cWindowskin, "selector", rpg_windowskin_get_selector, 0);
    rb_define_method(rb_cWindowskin, "selector=", rpg_windowskin_set_selector, 1);
    rb_define_method(rb_cWindowskin, "cursors", rpg_windowskin_get_cursors, 0);
    rb_define_method(rb_cWindowskin, "set_cursors", rpg_windowskin_set_cursors, 4);
    rb_define_method(rb_cWindowskin, "stretch_frame", rpg_windowskin_get_stretch_frame, 0);
    rb_define_method(rb_cWindowskin, "stretch_frame=", rpg_windowskin_set_stretch_frame, 1);
    rb_define_method(rb_cWindowskin, "stretch_background", rpg_windowskin_get_stretch_background, 0);
    rb_define_method(rb_cWindowskin, "stretch_background=", rpg_windowskin_set_stretch_background, 1);
    rb_define_method(rb_cWindowskin, "stretch_overlay", rpg_windowskin_get_stretch_overlay, 0);
    rb_define_method(rb_cWindowskin, "stretch_overlay=", rpg_windowskin_set_stretch_overlay, 1);
    rb_define_method(rb_cWindowskin, "frame_size", rpg_windowskin_get_frame_size, 0);
    rb_define_method(rb_cWindowskin, "frame_size=", rpg_windowskin_set_frame_size, 1);
    rb_define_method(rb_cWindowskin, "selector_size", rpg_windowskin_get_selector_size, 0);
    rb_define_method(rb_cWindowskin, "selector_size=", rpg_windowskin_set_selector_size, 1);
    rb_define_method(rb_cWindowskin, "arrows", rpg_windowskin_get_arrows, 0);
    rb_define_method(rb_cWindowskin, "set_arrows", rpg_windowskin_set_arrows, 4);
}

void rpg_windowskin_free(void *data) {
    RPGwindowskin *skin = data;
    if (skin->cursors) {
        xfree(skin->cursors);
    }
    xfree(data);
}

ALLOC_FUNC(rpg_windowskin_alloc, RPGwindowskin)

ATTR_READER(rpg_windowskin_get_frame_size, RPGwindowskin, frame_size, INT2NUM)
ATTR_READER(rpg_windowskin_get_selector_size, RPGwindowskin, selector_size, INT2NUM)
ATTR_READER(rpg_windowskin_get_stretch_frame, RPGwindowskin, stretch_frame, RB_BOOL)
ATTR_READER(rpg_windowskin_get_stretch_overlay, RPGwindowskin, stretch_overlay, RB_BOOL)
ATTR_READER(rpg_windowskin_get_stretch_background, RPGwindowskin, stretch_background, RB_BOOL)

ATTR_WRITER(rpg_windowskin_set_frame_size, RPGwindowskin, frame_size, NUM2INT)
ATTR_WRITER(rpg_windowskin_set_selector_size, RPGwindowskin, selector_size, NUM2INT)
ATTR_WRITER(rpg_windowskin_set_stretch_frame, RPGwindowskin, stretch_frame, (GLchar) RTEST)
ATTR_WRITER(rpg_windowskin_set_stretch_background, RPGwindowskin, stretch_background, (GLchar) RTEST)
ATTR_WRITER(rpg_windowskin_set_stretch_overlay, RPGwindowskin, stretch_overlay, (GLchar) RTEST)

GET_RECT_FUNC(rpg_windowskin_get_frame, frame)
GET_RECT_FUNC(rpg_windowskin_get_background, background)
GET_RECT_FUNC(rpg_windowskin_get_overlay, overlay)
GET_RECT_FUNC(rpg_windowskin_get_selector, selector)

SET_RECT_FUNC(rpg_windowskin_set_frame, frame)
SET_RECT_FUNC(rpg_windowskin_set_background, background)
SET_RECT_FUNC(rpg_windowskin_set_overlay, overlay)
SET_RECT_FUNC(rpg_windowskin_set_selector, selector)

static VALUE rpg_windowskin_get_cursors(VALUE self) {
    RPGwindowskin *w = DATA_PTR(self);
    VALUE ary = rb_ary_new_capa(CURSOR_COUNT);
    for (long i = 0; i < CURSOR_COUNT; i++) {
        RPGrect *r = ALLOC(RPGrect);
        memcpy(r, &w->cursors[i], sizeof(RPGrect));
        rb_ary_store(ary, i, Data_Wrap_Struct(rb_cRect, NULL, RUBY_DEFAULT_FREE, r));
    }
    return ary;
}

static VALUE rpg_windowskin_set_cursors(VALUE self, VALUE a1, VALUE a2, VALUE a3, VALUE a4) {
    RPGwindowskin *w = DATA_PTR(self);
    memcpy(&w->cursors[0], DATA_PTR(a1), sizeof(RPGrect));
    memcpy(&w->cursors[1], DATA_PTR(a2), sizeof(RPGrect));
    memcpy(&w->cursors[2], DATA_PTR(a3), sizeof(RPGrect));
    memcpy(&w->cursors[3], DATA_PTR(a4), sizeof(RPGrect));
    return Qnil;
}

static VALUE rpg_windowskin_get_arrows(VALUE self) {
    RPGwindowskin *w = DATA_PTR(self);
    RPGrect *arrows = (RPGrect*) &w->arrows;
    VALUE hash = rb_hash_new();
    const char *names[4] = { "up", "down", "left", "right" };
    for (int i = 0; i < 4; i++) {
        RPGrect *r = ALLOC(RPGrect);
        memcpy(r, &arrows[i], sizeof(RPGrect));
        rb_hash_aset(hash, STR2SYM(names[i]), Data_Wrap_Struct(rb_cRect, NULL, RUBY_DEFAULT_FREE, r));
    }
    return hash;
}

static VALUE rpg_windowskin_set_arrows(VALUE self, VALUE up, VALUE down, VALUE left, VALUE right) {
    RPGwindowskin *w = DATA_PTR(self);
    
}