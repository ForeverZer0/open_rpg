#include "open_rpg.h"

VALUE rb_mOpenRPG;
VALUE rb_mGame;
VALUE rb_mDisposable;
VALUE rb_eRPGError;

void Init_open_rpg(void) {
    rb_mOpenRPG = rb_define_module("OpenRPG");
    rb_mGame = rb_define_module_under(rb_mOpenRPG, "Game");
    rb_mDisposable = rb_define_module_under(rb_mOpenRPG, "Disposable");
    rb_eRPGError = rb_define_class_under(rb_mOpenRPG, "RPGError", rb_cObject);

    rpg_graphics_init(rb_mOpenRPG);
    rpg_input_init(rb_mOpenRPG);
    rpg_app_init(rb_mOpenRPG);

    rpg_bitmap_init(rb_mOpenRPG);
    rpg_renderable_init(rb_mOpenRPG);
    rpg_sprite_init(rb_mOpenRPG);
    rpg_viewport_init(rb_mOpenRPG);
    rpg_blend_init(rb_mOpenRPG);

    rpg_shader_init(rb_mOpenRPG);
    rpg_color_init(rb_mOpenRPG);
    rpg_tone_init(rb_mOpenRPG);
    rpg_font_init(rb_mOpenRPG);
    rpg_screen_init(rb_mOpenRPG);

    rpg_geometry_init(rb_mOpenRPG); 
    rpg_numerics_init(rb_mOpenRPG);

    rb_define_const(rb_mOpenRPG, "RMXP_COMPAT", RB_BOOL(RMXP_COMPAT));
    rb_define_singleton_method(rb_mGame, "update", rpg_empty_method, 0);
}

static VALUE rpg_empty_method(VALUE obj) {
    return obj;
}