#include "./transition.h"
#include "./graphics.h"
#include "./image.h"

VALUE rb_cTransition;

void rpg_transition_init(VALUE parent) {
    rb_cTransition = rb_define_module_under(parent, "Transition");
    rb_define_singleton_method(rb_cTransition, "execute", rpg_transition_execute, 2);


}

static VALUE rpg_transition_execute(VALUE module, VALUE shader, VALUE frames) {
    rb_need_block();

    if (NIL_P(shader)) {
        rb_yield(Qnil);
        return Qnil;
    }

    int f = NUM2INT(frames);
    // RPGshader *s = DATA_PTR(shader);
    // RPGimage *from = DATA_PTR(rpg_graphics_capture(rb_mGraphics));
    rb_yield(Qnil);
    // RPGimage *to = DATA_PTR(rpg_graphics_capture(rb_mGraphics));


    return Qnil;
}