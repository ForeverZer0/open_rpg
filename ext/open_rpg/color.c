#include "./rpg.h"
#include "./internal.h"

#define EPSILON 0.00039214f
#define R_SHIFT 24
#define G_SHIFT 16
#define B_SHIFT 8
#define A_SHIFT 0

VALUE rb_cColor;

ALLOC_FUNC(rpg_color_alloc, RPGcolor)

DUP_FUNC(rpg_color_dup, RPGcolor)
DUMP_FUNC(rpg_color_dump, RPGcolor)
LOAD_FUNC(rpg_color_load, RPGcolor)

ATTR_READER(rpg_color_get_r, RPGcolor, r, DBL2NUM)
ATTR_READER(rpg_color_get_g, RPGcolor, g, DBL2NUM)
ATTR_READER(rpg_color_get_b, RPGcolor, b, DBL2NUM)
ATTR_READER(rpg_color_get_a, RPGcolor, a, DBL2NUM)

static inline float rpg_color_component(VALUE c) {
    float value;
    if (FLONUM_P(c)) {
        value = NUM2FLT(c);
    } else {
        value = NUM2INT(c) / 255.0f;
    }
    return clampf(value, 0.0f, 1.0f);
}

static VALUE rpg_color_initialize(int argc, VALUE *argv, VALUE self) {
    VALUE r, g, b, alpha;
    rb_scan_args(argc, argv, "31", &r, &g, &b, &alpha);
    RPGcolor *color = DATA_PTR(self);
    color->r = rpg_color_component(r);
    color->g = rpg_color_component(g);
    color->b = rpg_color_component(b);
    color->a = NIL_P(alpha) ? 1.0f : rpg_color_component(alpha);
    return Qnil;
}

static VALUE rpg_color_to_i(VALUE self) {
    RPGcolor *color = DATA_PTR(self);
    GLuint r = (GLuint)roundf(color->r * 255.0f);
    GLuint g = (GLuint)roundf(color->g * 255.0f);
    GLuint b = (GLuint)roundf(color->b * 255.0f);
    GLuint a = (GLuint)roundf(color->a * 255.0f);

    return UINT2NUM((r << R_SHIFT) | (g << G_SHIFT) | (b << B_SHIFT) | (a << A_SHIFT));
}

static VALUE rpg_color_unpack(VALUE klass, VALUE value) {
    GLuint i = NUM2UINT(value);
    RPGcolor *color = ALLOC(RPGcolor);

    color->r = ((i >> R_SHIFT) & 0xFF) / 255.0f;
    color->g = ((i >> G_SHIFT) & 0xFF) / 255.0f;
    color->b = ((i >> B_SHIFT) & 0xFF) / 255.0f;
    color->a = ((i >> A_SHIFT) & 0xFF) / 255.0f;

    return Data_Wrap_Struct(klass, NULL, RUBY_DEFAULT_FREE, color);
}

static VALUE rpg_color_set(int argc, VALUE *argv, VALUE self) {
    VALUE r, g, b, a;
    rb_scan_args(argc, argv, "31", &r, &g, &b, &a);
    RPGcolor *color = DATA_PTR(self);
    color->r = rpg_color_component(r);
    color->g = rpg_color_component(g);
    color->b = rpg_color_component(b);
    color->a = NIL_P(a) ? 1.0f : rpg_color_component(a);
    return self;
}

static VALUE rpg_color_set_r(VALUE self, VALUE value) {
    RPGcolor *color = DATA_PTR(self);
    color->r = clampf(NUM2FLT(value), 0.0f, 1.0f);
    return value;
}

static VALUE rpg_color_set_g(VALUE self, VALUE value) {
    RPGcolor *color = DATA_PTR(self);
    color->g = clampf(NUM2FLT(value), 0.0f, 1.0f);
    return value;
}

static VALUE rpg_color_set_b(VALUE self, VALUE value) {
    RPGcolor *color = DATA_PTR(self);
    color->b = clampf(NUM2FLT(value), 0.0f, 1.0f);
    return value;
}

static VALUE rpg_color_set_a(VALUE self, VALUE value) {
    RPGcolor *color = DATA_PTR(self);
    color->a = clampf(NUM2FLT(value), 0.0f, 1.0f);
    return value;
}

static VALUE rpg_color_get_red(VALUE self) {
    RPGcolor *color = DATA_PTR(self);
    return INT2NUM((int)roundf(color->r * 255.0f));
}

static VALUE rpg_color_get_green(VALUE self) {
    RPGcolor *color = DATA_PTR(self);
    return INT2NUM((int)roundf(color->g * 255.0f));
}

static VALUE rpg_color_get_blue(VALUE self) {
    RPGcolor *color = DATA_PTR(self);
    return INT2NUM((int)roundf(color->b * 255.0f));
}

static VALUE rpg_color_get_alpha(VALUE self) {
    RPGcolor *color = DATA_PTR(self);
    return INT2NUM((int)roundf(color->a * 255.0f));
}

static VALUE rpg_color_set_red(VALUE self, VALUE value) {
    RPGcolor *color = DATA_PTR(self);
    color->r = clampf(NUM2INT(value) / 255.0f, 0.0f, 1.0f);
    return value;
}

static VALUE rpg_color_set_green(VALUE self, VALUE value) {
    RPGcolor *color = DATA_PTR(self);
    color->g = clampf(NUM2INT(value) / 255.0f, 0.0f, 1.0f);
    return value;
}

static VALUE rpg_color_set_blue(VALUE self, VALUE value) {
    RPGcolor *color = DATA_PTR(self);
    color->b = clampf(NUM2INT(value) / 255.0f, 0.0f, 1.0f);
    return value;
}

static VALUE rpg_color_set_alpha(VALUE self, VALUE value) {
    RPGcolor *color = DATA_PTR(self);
    color->a = clampf(NUM2INT(value) / 255.0f, 0.0f, 1.0f);
    return value;
}

static VALUE rpg_color_equal(VALUE self, VALUE other) {
    if (CLASS_OF(self) != CLASS_OF(other)) {
        return Qfalse;
    }
    RPGcolor *c1 = DATA_PTR(self);
    RPGcolor *c2 = DATA_PTR(other);

    return (fabsf(c1->r - c2->r) < EPSILON && fabsf(c1->g - c2->g) < EPSILON && fabsf(c1->b - c2->b) < EPSILON &&
            fabsf(c1->a - c2->a) < EPSILON)
               ? Qtrue
               : Qfalse;
}

static VALUE rpg_color_inspect(VALUE self) {
    RPGcolor *c = DATA_PTR(self);
    return rb_sprintf("<Color: r:%d g:%d b:%d alpha:%d>", (int)roundf(c->r * 255.0f), (int)roundf(c->g * 255.0f),
                      (int)roundf(c->b * 255.0f), (int)roundf(c->a * 255.0f));
}

static VALUE rpg_color_rgb(VALUE self) {
    RPGcolor *color = DATA_PTR(self);
    RPGvec3 *vec = ALLOC(RPGvec3);
    memcpy(vec, color, sizeof(RPGvec3));
    return Data_Wrap_Struct(rb_cVec3, NULL, RUBY_DEFAULT_FREE, vec);
}

static VALUE rpg_color_rgba(VALUE self) {
    RPGcolor *color = DATA_PTR(self);
    RPGvec4 *vec = ALLOC(RPGvec4);
    memcpy(vec, color, sizeof(RPGvec4));
    return Data_Wrap_Struct(rb_cVec4, NULL, RUBY_DEFAULT_FREE, vec);
}

static VALUE rpg_color_bgr(VALUE self) {
    RPGcolor *color = DATA_PTR(self);
    RPGvec3 *vec = ALLOC(RPGvec3);
    vec->x = color->b;
    vec->y = color->g;
    vec->z = color->r;
    return Data_Wrap_Struct(rb_cVec3, NULL, RUBY_DEFAULT_FREE, vec);
}

static VALUE rpg_color_bgra(VALUE self) {
    RPGcolor *color = DATA_PTR(self);
    RPGvec4 *vec = ALLOC(RPGvec4);
    vec->x = color->b;
    vec->y = color->g;
    vec->z = color->r;
    vec->w = color->a;
    return Data_Wrap_Struct(rb_cVec4, NULL, RUBY_DEFAULT_FREE, vec);
}

static VALUE rpg_color_hsv(VALUE self) {
    RPGcolor *color = DATA_PTR(self);
    RPGvec3 *vec = ALLOC(RPGvec3);
    memset(vec, 0, sizeof(RPGvec3));

    float max = fmaxf(color->r, fmaxf(color->g, color->b));
    float min = fminf(color->r, fminf(color->g, color->b));
    float diff = max - min;
    float delta = 1.0f / diff;

    if (FLT_EQL(max, color->r)) {
        vec->x = (color->g - color->b) * delta;
    } else if (FLT_EQL(max, color->g)) {
        vec->x = 2 + (color->b - color->r) * delta;
    } else {
        vec->x = 4 + (color->r - color->g) * delta;
    }

    vec->x *= 60.0f;
    if (vec->x < 0.0f) {
        vec->x += 360.0f;
    }

    if (diff > __FLT_EPSILON__) {
        vec->y = diff / max;
        vec->z = max;
    }
    return Data_Wrap_Struct(rb_cVec3, NULL, RUBY_DEFAULT_FREE, vec);
}

static VALUE rpg_color_hsl(VALUE self) {
    RPGcolor *color = DATA_PTR(self);
    RPGvec3 *vec = ALLOC(RPGvec3);
    memset(vec, 0, sizeof(RPGvec3));

    float max = fmaxf(color->r, fmaxf(color->g, color->b));
    float min = fminf(color->r, fminf(color->g, color->b));
    float diff = max - min;
    float delta = 1.0f / diff;

    if (FLT_EQL(max, color->r)) {
        vec->x = (color->g - color->b) * delta;
    } else if (FLT_EQL(max, color->g)) {
        vec->x = 2 + (color->b - color->r) * delta;
    } else {
        vec->x = 4 + (color->r - color->g) * delta;
    }

    vec->x *= 60;
    if (vec->x < 0.0f) {
        vec->x += 360.0f;
    }

    if (diff > __FLT_EPSILON__) {
        vec->z = (max + min) * 0.5f;
        if (vec->z > 0.5f)
            vec->y = (max - min) / (2 - max - min);
        else
            vec->y = (max - min) / (max + min);
    }
    return Data_Wrap_Struct(rb_cVec3, NULL, RUBY_DEFAULT_FREE, vec);
}

static VALUE rpg_color_lerp_bang(VALUE self, VALUE other, VALUE amount) {
    RPGcolor *c1 = DATA_PTR(self);
    RPGcolor *c2 = DATA_PTR(other);
    float w = clampf(NUM2FLT(amount), 0.0f, 1.0f);
    c1->r = c1->r + (c2->r - c1->r) * w;
    c1->g = c1->g + (c2->g - c1->g) * w;
    c1->b = c1->b + (c2->b - c1->b) * w;
    c1->a = c1->a + (c2->a - c1->a) * w;
    return self;
}

static VALUE rpg_color_lerp(VALUE self, VALUE other, VALUE amount) {
    VALUE clone = rpg_color_dup(self);
    return rpg_color_lerp_bang(clone, other, amount);
}

static VALUE rpg_color_html(VALUE self) {
    RPGcolor *color = DATA_PTR(self);
    return rb_sprintf("#%02x%02x%02x%02x", (int)roundf(color->r * 255.0f), (int)roundf(color->g * 255.0f), (int)roundf(color->b * 255.0f),
                      (int)roundf(color->a * 255.0f));
}

static VALUE rpg_color_from_hsv(int argc, VALUE *argv, VALUE klass) {
    VALUE h, s, v, a;
    rb_scan_args(argc, argv, "31", &h, &s, &v, &a);
    RPGcolor *color = ALLOC(RPGcolor);

    float hue = clampf(NUM2FLT(h), 0.0f, 360.0f);
    float saturation = clampf(NUM2FLT(s), 0.0f, 1.0f);
    float value = clampf(NUM2FLT(v), 0.0f, 1.0f);
    color->a = NIL_P(a) ? 1.0f : clampf(NUM2FLT(a), 0.0f, 1.0f);

    if (value < __FLT_EPSILON__) {
        color->r = 0.0f;
        color->g = 0.0f;
        color->b = 0.0f;
    } else if (saturation < __FLT_EPSILON__) {
        color->r = value;
        color->g = value;
        color->b = value;
    } else {
        float hf = hue / 60.0f;
        int i = (int)floorf(hf);
        float f = hf - i;
        float pv = value * (1.0f - saturation);
        float qv = value * (1.0f - saturation * f);
        float tv = value * (1.0f - saturation * (1.0f - f));

        switch (i) {
            case 0:
            case 6: {
                color->r = value;
                color->g = tv;
                color->b = pv;
                break;
            }
            case 1: {
                color->r = qv;
                color->g = value;
                color->b = pv;
                break;
            }
            case 2: {
                color->r = pv;
                color->g = value;
                color->b = tv;
                break;
            }
            case 3: {
                color->r = pv;
                color->g = qv;
                color->b = value;
                break;
            }
            case 4: {
                color->r = tv;
                color->g = pv;
                color->b = value;
                break;
            }
            case 5: {
                color->r = value;
                color->g = pv;
                color->b = qv;
                break;
            }
            case -1: {
                color->r = value;
                color->g = pv;
                color->b = qv;
                break;
            }
            default: {
                color->r = value;
                color->g = value;
                color->b;
                break;
            }
        }
    }
    return Data_Wrap_Struct(klass, NULL, RUBY_DEFAULT_FREE, color);
}

static inline float oqh_rgb(float q1, float q2, float h) {
    while (h > 360.0f) {
        h -= 360.0f;
    }
    while (h < 0.0f) {
        h += 360.0f;
    }
    if (h < 60.0f) {
        return q1 + (q2 - q1) * h / 60.0f;
    }
    if (h < 180.0f) {
        return q2;
    }
    if (h < 240.0f) {
        return q1 + (q2 - q1) * (240.0f - h) / 60.0f;
    }
    return q1;
}

static VALUE rpg_color_from_hsl(int argc, VALUE *argv, VALUE klass) {
    VALUE h, s, l, a;
    rb_scan_args(argc, argv, "31", &h, &s, &l, &a);
    RPGcolor *color = ALLOC(RPGcolor);

    float hue = clampf(NUM2FLT(h), 0.0f, 360.0f);
    float saturation = clampf(NUM2FLT(s), 0.0f, 1.0f);
    float lightness = clampf(NUM2FLT(l), 0.0f, 1.0f);
    color->a = NIL_P(a) ? 1.0f : clampf(NUM2FLT(a), 0.0f, 1.0f);

    float p2 = lightness <= 0.5f ? lightness + (1 + saturation) : lightness + saturation - lightness * saturation;

    float p1 = 2 * lightness - p2;
    if (saturation < __FLT_EPSILON__) {
        color->r = lightness;
        color->g = lightness;
        color->b = lightness;
    } else {
        color->r = oqh_rgb(p1, p2, hue + 120.0f);
        color->g = oqh_rgb(p1, p2, hue);
        color->b = oqh_rgb(p1, p2, hue - 120);
    }
    return Data_Wrap_Struct(klass, NULL, RUBY_DEFAULT_FREE, color);
}

static VALUE rpg_color_invert_bang(int argc, VALUE *argv, VALUE self) {
    VALUE alpha;
    rb_scan_args(argc, argv, "01", &alpha);

    RPGcolor *color = DATA_PTR(self);
    color->r = 1.0f - color->r;
    color->g = 1.0f - color->g;
    color->b = 1.0f - color->b;
    if (RTEST(alpha)) {
        color->a = 1.0f - color->a;
    }
    return self;
}

static VALUE rpg_color_invert(int argc, VALUE *argv, VALUE self) {
    VALUE clone = rpg_color_dup(self);
    return rpg_color_invert_bang(argc, argv, clone);
}

void rpg_color_init(VALUE parent) {
    rb_cColor = rb_define_class_under(parent, "Color", rb_cObject);

    rb_define_alloc_func(rb_cColor, rpg_color_alloc);
    rb_define_method(rb_cColor, "initialize", rpg_color_initialize, -1);
    rb_define_method(rb_cColor, "set", rpg_color_set, -1);

    rb_define_method(rb_cColor, "r", rpg_color_get_r, 0);
    rb_define_method(rb_cColor, "g", rpg_color_get_g, 0);
    rb_define_method(rb_cColor, "b", rpg_color_get_b, 0);
    rb_define_method(rb_cColor, "a", rpg_color_get_a, 0);
    rb_define_method(rb_cColor, "r=", rpg_color_set_r, 1);
    rb_define_method(rb_cColor, "g=", rpg_color_set_g, 1);
    rb_define_method(rb_cColor, "b=", rpg_color_set_b, 1);
    rb_define_method(rb_cColor, "a=", rpg_color_set_a, 1);

    rb_define_method(rb_cColor, "red", rpg_color_get_red, 0);
    rb_define_method(rb_cColor, "green", rpg_color_get_green, 0);
    rb_define_method(rb_cColor, "blue", rpg_color_get_blue, 0);
    rb_define_method(rb_cColor, "alpha", rpg_color_get_alpha, 0);
    rb_define_method(rb_cColor, "red=", rpg_color_set_red, 1);
    rb_define_method(rb_cColor, "green=", rpg_color_set_green, 1);
    rb_define_method(rb_cColor, "blue=", rpg_color_set_blue, 1);
    rb_define_method(rb_cColor, "alpha=", rpg_color_set_alpha, 1);

    rb_define_method(rb_cColor, "rgb", rpg_color_rgb, 0);
    rb_define_method(rb_cColor, "rgba", rpg_color_rgba, 0);
    rb_define_method(rb_cColor, "bgr", rpg_color_bgr, 0);
    rb_define_method(rb_cColor, "bgra", rpg_color_bgra, 0);
    rb_define_method(rb_cColor, "hsv", rpg_color_hsv, 0);
    rb_define_method(rb_cColor, "hsb", rpg_color_hsv, 0);
    rb_define_method(rb_cColor, "hsl", rpg_color_hsl, 0);
    rb_define_method(rb_cColor, "html", rpg_color_html, 0);

    rb_define_method(rb_cColor, "invert", rpg_color_invert, -1);
    rb_define_method(rb_cColor, "invert!", rpg_color_invert_bang, -1);
    rb_define_method(rb_cColor, "to_i", rpg_color_to_i, 0);
    rb_define_method(rb_cColor, "dup", rpg_color_dup, 0);
    rb_define_method(rb_cColor, "inspect", rpg_color_inspect, 0);
    rb_define_method(rb_cColor, "==", rpg_color_equal, 1);
    rb_define_method(rb_cColor, "_dump", rpg_color_dump, -1);

    rb_define_singleton_method(rb_cColor, "_load", rpg_color_load, 1);
    rb_define_singleton_method(rb_cColor, "from_hsb", rpg_color_from_hsv, -1);
    rb_define_singleton_method(rb_cColor, "from_hsv", rpg_color_from_hsv, -1);
    rb_define_singleton_method(rb_cColor, "from_hsl", rpg_color_from_hsl, -1);
    rb_define_singleton_method(rb_cColor, "unpack", rpg_color_unpack, 1);
}