#include "screen.h"

VALUE rb_mScreen;

void rpg_screen_init(VALUE parent) {
    rb_mScreen = rb_define_module_under(parent, "Screen");
    rb_define_singleton_method(rb_mScreen, "tone", rpg_screen_get_tone, 0);
    rb_define_singleton_method(rb_mScreen, "tone=", rpg_screen_set_tone, 1);
    rb_define_singleton_method(rb_mScreen, "z", rpg_screen_get_z, 0);
    rb_define_singleton_method(rb_mScreen, "z=", rpg_screen_set_z, 1);
    rb_define_singleton_method(rb_mScreen, "update", rpg_screen_update, 0);

    rb_define_singleton_method(rb_mScreen, "transition", rpg_screen_transition, 3);
    rb_define_singleton_method(rb_mScreen, "reset", rpg_screen_reset, 0);
}

static VALUE rpg_screen_get_tone(VALUE module) {
    RPGtone *tone = ALLOC(RPGtone);
    glGetUniformfv(_program, _screen, (float*) tone);
    return Data_Wrap_Struct(rb_cTone, NULL, NULL, &tone);
}

static VALUE rpg_screen_set_tone(VALUE module, VALUE value) {
    RPGtone *tone = DATA_PTR(value);
    glUseProgram(_program);
    glUniform4f(_screen, tone->r, tone->g, tone->b, tone->gr);
    return value;
}

static VALUE rpg_screen_get_z(VALUE module) {
    GLint z;
    glGetUniformiv(_program, _screen_z, &z);
    return INT2NUM(z);
}

static VALUE rpg_screen_set_z(VALUE module, VALUE value) {
    glUseProgram(_program);
    glUniform1i(_screen_z, NUM2INT(value));
    return Qnil;
}

static VALUE rpg_screen_update(VALUE module) {


    return Qnil;
}

static inline void rpg_gen_steps(float start, float target, GLuint n, float *results) {

}

static VALUE rpg_screen_transition(VALUE module, VALUE tone, VALUE frames, VALUE num_steps) {

    
    return Qnil;
}

static VALUE rpg_screen_reset(VALUE module) {
    glUseProgram(_program);
    glUniform4f(_screen, 0.0f, 0.0f, 0.0f, 0.0f);

}