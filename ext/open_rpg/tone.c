#include "./tone.h"

#define EPSILON 0.00039214f

VALUE rb_cTone;

void rpg_tone_init(VALUE parent) {
    rb_cTone = rb_define_class_under(parent, "Tone", rb_cObject);

    rb_define_alloc_func(rb_cTone, rpg_tone_alloc);
    rb_define_method(rb_cTone, "initialize", rpg_tone_initialize, -1);
    rb_define_method(rb_cTone, "set", rpg_tone_set, -1);
    rb_define_method(rb_cTone, "inspect", rpg_tone_inspect, 0);

    rb_define_method(rb_cTone, "r", rpg_tone_get_r, 0);
    rb_define_method(rb_cTone, "g", rpg_tone_get_g, 0);
    rb_define_method(rb_cTone, "b", rpg_tone_get_b, 0);
    rb_define_method(rb_cTone, "gr", rpg_tone_get_gr, 0);
    rb_define_method(rb_cTone, "r=", rpg_tone_set_r, 1);
    rb_define_method(rb_cTone, "g=", rpg_tone_set_g, 1);
    rb_define_method(rb_cTone, "b=", rpg_tone_set_b, 1);
    rb_define_method(rb_cTone, "gr=", rpg_tone_set_gr, 1);

    rb_define_method(rb_cTone, "red", rpg_tone_get_red, 0);
    rb_define_method(rb_cTone, "green", rpg_tone_get_green, 0);
    rb_define_method(rb_cTone, "blue", rpg_tone_get_blue, 0);
    rb_define_method(rb_cTone, "gray", rpg_tone_get_gray, 0);
    rb_define_method(rb_cTone, "red=", rpg_tone_set_red, 1);
    rb_define_method(rb_cTone, "green=", rpg_tone_set_green, 1);
    rb_define_method(rb_cTone, "blue=", rpg_tone_set_blue, 1);
    rb_define_method(rb_cTone, "gray=", rpg_tone_set_gray, 1);

    rb_define_method(rb_cTone, "dup", rpg_tone_dup, 0);
    rb_define_method(rb_cTone, "==", rpg_tone_equal, 1);
    rb_define_method(rb_cTone, "_dump", rpg_tone_dump, -1);
    rb_define_singleton_method(rb_cTone, "_load", rpg_tone_load, 1);
}

ALLOC_FUNC(rpg_tone_alloc, RPGtone)
DUP_FUNC(rpg_tone_dup, RPGtone)
DUMP_FUNC(rpg_tone_dump, RPGtone)
LOAD_FUNC(rpg_tone_load, RPGtone)

ATTR_READER(rpg_tone_get_r, RPGtone, r, DBL2NUM)
ATTR_READER(rpg_tone_get_g, RPGtone, g, DBL2NUM)
ATTR_READER(rpg_tone_get_b, RPGtone, b, DBL2NUM)
ATTR_READER(rpg_tone_get_gr, RPGtone, gr, DBL2NUM)

static inline float rpg_tone_component(VALUE c, float min, float max) {
    float value;
    if (FLONUM_P(c)) {
        value = NUM2FLT(c);
    }
    else {
        value = NUM2INT(c) / 255.0f;
    }
    return clampf(value, min, max);
}

static VALUE rpg_tone_initialize(int argc, VALUE *argv, VALUE self) {
    VALUE r, g, b, gray;
    rb_scan_args(argc, argv, "31", &r, &g, &b, &gray);
    RPGtone *tone = DATA_PTR(self);
    tone->r = rpg_tone_component(r, -1.0f, 1.0f);
    tone->g = rpg_tone_component(g, -1.0f, 1.0f);
    tone->b = rpg_tone_component(b, -1.0f, 1.0f);
    tone->gr = NIL_P(gray) ? 0.0f : rpg_tone_component(gray, 0.0f, 1.0f);
    return Qnil;
}

static VALUE rpg_tone_set(int argc, VALUE *argv, VALUE self) {
    VALUE r, g, b, gray;
    rb_scan_args(argc, argv, "31", &r, &g, &b, &gray);
    RPGtone *tone = DATA_PTR(self);
    tone->r = rpg_tone_component(r, -1.0f, 1.0f);
    tone->g = rpg_tone_component(g, -1.0f, 1.0f);
    tone->b = rpg_tone_component(b, -1.0f, 1.0f);
    tone->gr = NIL_P(gray) ? 1.0f : rpg_tone_component(gray, 0.0f, 1.0f);
    return self;
}

static VALUE rpg_tone_inspect(VALUE self) {
    RPGtone *t = DATA_PTR(self);
    return rb_sprintf("<Tone: r:%d g:%d b:%d gray:%d>",
        (int) roundf(t->r * 255.0f),
        (int) roundf(t->g * 255.0f),
        (int) roundf(t->b * 255.0f),
        (int) roundf(t->gr * 255.0f)
    );
}

static VALUE rpg_tone_set_r(VALUE self, VALUE value) {
    RPGtone *tone = DATA_PTR(self);
    tone->r = clampf(NUM2FLT(value), -1.0f, 1.0f);
    return value;
}

static VALUE rpg_tone_set_g(VALUE self, VALUE value) {
    RPGtone *tone = DATA_PTR(self);
    tone->g = clampf(NUM2FLT(value), -1.0f, 1.0f);
    return value;
}

static VALUE rpg_tone_set_b(VALUE self, VALUE value) {
    RPGtone *tone = DATA_PTR(self);
    tone->b = clampf(NUM2FLT(value), -1.0f, 1.0f);
    return value;
}

static VALUE rpg_tone_set_gr(VALUE self, VALUE value) {
    RPGtone *tone = DATA_PTR(self);
    tone->gr = clampf(NUM2FLT(value), 0.0f, 1.0f);
    return value;
}

static VALUE rpg_tone_get_red(VALUE self) {
    RPGtone *tone = DATA_PTR(self);
    return INT2NUM((int) roundf(tone->r * 255.0f));
}

static VALUE rpg_tone_get_green(VALUE self) {
    RPGtone *tone = DATA_PTR(self);
    return INT2NUM((int) roundf(tone->g * 255.0f));
}

static VALUE rpg_tone_get_blue(VALUE self) {
    RPGtone *tone = DATA_PTR(self);
    return INT2NUM((int) roundf(tone->b * 255.0f));
}

static VALUE rpg_tone_get_gray(VALUE self) {
    RPGtone *tone = DATA_PTR(self);
    return INT2NUM((int) roundf(tone->gr * 255.0f));
}

static VALUE rpg_tone_set_red(VALUE self, VALUE value) {
    RPGtone *tone = DATA_PTR(self);
    tone->r = clampf(NUM2INT(value) / 255.0f, -1.0f, 1.0f);
    return value;
}

static VALUE rpg_tone_set_green(VALUE self, VALUE value) {
    RPGtone *tone = DATA_PTR(self);
    tone->g = clampf(NUM2INT(value) / 255.0f, -1.0f, 1.0f);
    return value;
}

static VALUE rpg_tone_set_blue(VALUE self, VALUE value) {
    RPGtone *tone = DATA_PTR(self);
    tone->b = clampf(NUM2INT(value) / 255.0f, -1.0f, 1.0f);
    return value;
}

static VALUE rpg_tone_set_gray(VALUE self, VALUE value) {
    RPGtone *tone = DATA_PTR(self);
    tone->gr = clampf(NUM2INT(value) / 255.0f, 0.0f, 1.0f);
    return value;
}

static VALUE rpg_tone_equal(VALUE self, VALUE other) {
    if (CLASS_OF(self) != CLASS_OF(other)) {
        return Qfalse;
    }
    RPGtone *t1 = DATA_PTR(self);
    RPGtone *t2 = DATA_PTR(other);

    return (fabsf(t1->r - t2->r) < EPSILON && 
        fabsf(t1->g - t2->g) < EPSILON && 
        fabsf(t1->b - t2->b) < EPSILON && 
        fabsf(t1->gr - t2->gr) < EPSILON) ? Qtrue : Qfalse;
}