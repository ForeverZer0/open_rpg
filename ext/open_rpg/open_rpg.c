#include "open_rpg.h"

VALUE rb_mOpenRPG;
VALUE rb_mDisposable;
VALUE rb_eRPGError;

void Init_open_rpg(void) {
    rb_mOpenRPG = rb_define_module("OpenRPG");
    rb_mDisposable = rb_define_module_under(rb_mOpenRPG, "Disposable");
    rb_eRPGError = rb_define_class_under(rb_mOpenRPG, "RPGError", rb_cObject);

    rpg_graphics_init(rb_mOpenRPG);
    rpg_input_init(rb_mOpenRPG);
    rpg_app_init(rb_mOpenRPG);

    rpg_bitmap_init(rb_mOpenRPG);
    rpg_sprite_init(rb_mOpenRPG);
    rpg_viewport_init(rb_mOpenRPG);
    
    rpg_shader_init(rb_mOpenRPG);
    rpg_color_init(rb_mOpenRPG);
    rpg_tone_init(rb_mOpenRPG);
    rpg_font_init(rb_mOpenRPG);
    rpg_screen_init(rb_mOpenRPG);

    rpg_geometry_init(rb_mOpenRPG); 
    rpg_numerics_init(rb_mOpenRPG);

    rb_define_singleton_method(rb_mOpenRPG, "terminate", open_rpg_terminate, 0);
}

VALUE open_rpg_terminate(VALUE module) {
    glfwTerminate();
    rpg_font_terminate();
    // TODO: Additional cleanup

    return Qnil;
}
