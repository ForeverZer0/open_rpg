#include "./blend.h"

VALUE rb_cBlend;

void rpg_blend_init(VALUE parent) {
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
}

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
    // TODO: Implement to match RMXP
    return Data_Wrap_Struct(rb_cBlend, NULL, RUBY_DEFAULT_FREE, blend);
}

static VALUE rpg_blend_subtract(VALUE klass) {
    RPGblend *blend = ALLOC(RPGblend);
    // TODO: Implement to match RMXP
    return Data_Wrap_Struct(rb_cBlend, NULL, RUBY_DEFAULT_FREE, blend);
}