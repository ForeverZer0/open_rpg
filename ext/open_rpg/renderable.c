#include "./renderable.h"
#include "./internal.h"

VALUE rb_cRenderable;
VALUE rb_cBlend;

#pragma region Blend

ALLOC_FUNC(rpg_blend_alloc, RPGblend)
ATTR_READER(rpg_blend_get_equation, RPGblend, equation, INT2NUM)
ATTR_READER(rpg_blend_get_sfactor, RPGblend, src_factor, INT2NUM)
ATTR_READER(rpg_blend_get_dfactor, RPGblend, dst_factor, INT2NUM)
ATTR_WRITER(rpg_blend_set_equation, RPGblend, equation, NUM2INT)
ATTR_WRITER(rpg_blend_set_sfactor, RPGblend, src_factor, NUM2INT)
ATTR_WRITER(rpg_blend_set_dfactor, RPGblend, dst_factor, NUM2INT)

static VALUE rpg_blend_initialize(VALUE self, VALUE equation, VALUE sfactor, VALUE dfactor) {
    RPGblend *blend = DATA_PTR(self);
    blend->equation = NUM2INT(equation);
    blend->src_factor = NUM2INT(sfactor);
    blend->dst_factor = NUM2INT(dfactor);
    return Qnil;
}

static VALUE rpg_blend_equal(VALUE self, VALUE obj) {
    if (CLASS_OF(self) != CLASS_OF(obj)) {
        return Qfalse;
    }
    RPGblend *a = DATA_PTR(self);
    RPGblend *b = DATA_PTR(obj);
    return RB_BOOL(a->equation == b->equation && a->src_factor == b->src_factor && a->dst_factor == b->dst_factor);
}

static VALUE rpg_blend_normal(VALUE klass) {
    RPGblend *blend = ALLOC(RPGblend);
    blend->equation = GL_FUNC_ADD;
    blend->src_factor = GL_SRC_ALPHA;
    blend->dst_factor = GL_ONE_MINUS_SRC_ALPHA;
    return Data_Wrap_Struct(rb_cBlend, NULL, RUBY_DEFAULT_FREE, blend);
}

static VALUE rpg_blend_add(VALUE klass) {
    RPGblend *blend = ALLOC(RPGblend);
    blend->equation = GL_FUNC_ADD;
    blend->src_factor = GL_SRC_ALPHA;
    blend->dst_factor = GL_DST_ALPHA;
    return Data_Wrap_Struct(rb_cBlend, NULL, RUBY_DEFAULT_FREE, blend);
}

static VALUE rpg_blend_subtract(VALUE klass) {
    RPGblend *blend = ALLOC(RPGblend);
    blend->equation = GL_FUNC_REVERSE_SUBTRACT;
    blend->src_factor = GL_ONE_MINUS_SRC_ALPHA;
    blend->dst_factor = GL_DST_ALPHA;
    return Data_Wrap_Struct(rb_cBlend, NULL, RUBY_DEFAULT_FREE, blend);
}

#pragma endregion Blend

static VALUE rpg_renderable_alloc(VALUE klass) {
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
ATTR_WRITER(rpg_renderable_set_ox, RPGrenderable, ox, NUM2INT)
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
    const float f = 180.0f / RPG_PI;
    return DBL2NUM((f * renderable->rotation.radians));
}

static VALUE rpg_renderable_set_angle(VALUE self, VALUE value) {
    RPGrenderable *renderable = DATA_PTR(self);
    const float f = RPG_PI / 180.0f;
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
    RPGvec2 *vec = ALLOC(RPGvec2);
    memcpy(vec, &renderable->scale, sizeof(RPGvec2));
    return Data_Wrap_Struct(rb_cVec2, NULL, RUBY_DEFAULT_FREE, vec);
}

static VALUE rpg_renderable_set_scale(VALUE self, VALUE value) {
    RPGrenderable *renderable = DATA_PTR(self);
    RPGvec2 *vec = DATA_PTR(value);
    renderable->scale.x = vec->x;
    renderable->scale.y = vec->y;
    renderable->updated = GL_TRUE;
    return value;
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
    return Data_Wrap_Struct(rb_cBlend, NULL, NULL, &renderable->blend);
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

    // Blend
    rb_cBlend = rb_define_class_under(parent, "Blend", rb_cObject);
    rb_define_alloc_func(rb_cBlend, rpg_blend_alloc);
    rb_define_method(rb_cBlend, "initialize", rpg_blend_initialize, 3);

    rb_define_method(rb_cBlend, "equation", rpg_blend_get_equation, 0);
    rb_define_method(rb_cBlend, "src_factor", rpg_blend_get_sfactor, 0);
    rb_define_method(rb_cBlend, "dst_factor", rpg_blend_get_dfactor, 0);
    rb_define_method(rb_cBlend, "equation=", rpg_blend_set_equation, 1);
    rb_define_method(rb_cBlend, "src_factor=", rpg_blend_set_sfactor, 1);
    rb_define_method(rb_cBlend, "dst_factor=", rpg_blend_set_dfactor, 1);
    rb_define_method(rb_cBlend, "==", rpg_blend_equal, 1);

    rb_define_singleton_method(rb_cBlend, "normal", rpg_blend_normal, 0);
    rb_define_singleton_method(rb_cBlend, "add", rpg_blend_add, 0);
    rb_define_singleton_method(rb_cBlend, "subtract", rpg_blend_subtract, 0);

    rb_define_const(rb_cBlend, "ZERO", INT2NUM(GL_ZERO));
    rb_define_const(rb_cBlend, "ONE", INT2NUM(GL_ONE));
    rb_define_const(rb_cBlend, "SRC_COLOR", INT2NUM(GL_SRC_COLOR));
    rb_define_const(rb_cBlend, "ONE_MINUS_SRC_COLOR", INT2NUM(GL_ONE_MINUS_SRC_COLOR));
    rb_define_const(rb_cBlend, "DST_COLOR", INT2NUM(GL_DST_COLOR));
    rb_define_const(rb_cBlend, "ONE_MINUS_DST_COLOR", INT2NUM(GL_ONE_MINUS_DST_COLOR));
    rb_define_const(rb_cBlend, "SRC_ALPHA", INT2NUM(GL_SRC_ALPHA));
    rb_define_const(rb_cBlend, "ONE_MINUS_SRC_ALPHA", INT2NUM(GL_ONE_MINUS_SRC_ALPHA));
    rb_define_const(rb_cBlend, "DST_ALPHA", INT2NUM(GL_DST_ALPHA));
    rb_define_const(rb_cBlend, "ONE_MINUS_DST_ALPHA", INT2NUM(GL_ONE_MINUS_DST_ALPHA));
    rb_define_const(rb_cBlend, "CONSTANT_COLOR", INT2NUM(GL_CONSTANT_COLOR));
    rb_define_const(rb_cBlend, "ONE_MINUS_CONSTANT_COLOR", INT2NUM(GL_ONE_MINUS_CONSTANT_COLOR));
    rb_define_const(rb_cBlend, "CONSTANT_ALPHA", INT2NUM(GL_CONSTANT_ALPHA));
    rb_define_const(rb_cBlend, "ONE_MINUS_CONSTANT_ALPHA", INT2NUM(GL_ONE_MINUS_CONSTANT_ALPHA));
    rb_define_const(rb_cBlend, "SRC_ALPHA_SATURATE", INT2NUM(GL_SRC_ALPHA_SATURATE));
    rb_define_const(rb_cBlend, "FUNC_ADD", INT2NUM(GL_FUNC_ADD));
    rb_define_const(rb_cBlend, "FUNC_SUBTRACT", INT2NUM(GL_FUNC_SUBTRACT));
    rb_define_const(rb_cBlend, "FUNC_REVERSE_SUBTRACT", INT2NUM(GL_FUNC_REVERSE_SUBTRACT));

    rpg_sprite_init(parent);
    rpg_sprite_sheet_init(parent);
    rpg_plane_init(parent);
    rpg_viewport_init(parent);
}