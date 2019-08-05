#include "./open_rpg.h"

VALUE rb_mOpenRPG;
VALUE rb_mGame;
VALUE rb_eRPGError;
VALUE ENOENT;

void Init_open_rpg(void) {
    rb_mOpenRPG = rb_define_module("OpenRPG");
    rb_mGame = rb_define_module_under(rb_mOpenRPG, "Game");
    rb_eRPGError = rb_define_class_under(rb_mOpenRPG, "RPGError", rb_eStandardError);

    rpg_graphics_init(rb_mOpenRPG);
    rpg_input_init(rb_mOpenRPG);
    rpg_app_init(rb_mOpenRPG);

    rpg_image_init(rb_mOpenRPG);
    rpg_renderable_init(rb_mOpenRPG);
    rpg_sprite_init(rb_mOpenRPG);
    rpg_viewport_init(rb_mOpenRPG);
    rpg_blend_init(rb_mOpenRPG);

    rpg_shader_init(rb_mOpenRPG);
    rpg_color_init(rb_mOpenRPG);
    rpg_tone_init(rb_mOpenRPG);
    rpg_font_init(rb_mOpenRPG);
    rpg_table_init(rb_mOpenRPG);

    rpg_geometry_init(rb_mOpenRPG); 
    rpg_numerics_init(rb_mOpenRPG);

    ENOENT = rb_const_get(rb_mErrno, rb_intern("ENOENT"));
    rb_define_singleton_method(rb_mGame, "update", rpg_empty_method, 0);

    VALUE dir = rb_define_module_under(rb_mOpenRPG, "Direction");
    rb_define_const(dir, "NONE", INT2NUM(RPG_NONE));
    rb_define_const(dir, "LEFT", INT2NUM(RPG_LEFT));
    rb_define_const(dir, "TOP", INT2NUM(RPG_TOP));
    rb_define_const(dir, "RIGHT", INT2NUM(RPG_RIGHT));
    rb_define_const(dir, "BOTTOM", INT2NUM(RPG_BOTTOM));
    rb_define_const(dir, "TOP_LEFT", INT2NUM(RPG_TOP_LEFT));
    rb_define_const(dir, "TOP_RIGHT", INT2NUM(RPG_TOP_RIGHT));
    rb_define_const(dir, "BOTTOM_LEFT", INT2NUM(RPG_BOTTOM_LEFT));
    rb_define_const(dir, "BOTTOM_RIGHT", INT2NUM(RPG_BOTTOM_RIGHT));
    rb_define_const(dir, "WEST", INT2NUM(RPG_WEST));
    rb_define_const(dir, "NORTH", INT2NUM(RPG_NORTH));
    rb_define_const(dir, "EAST", INT2NUM(RPG_EAST));
    rb_define_const(dir, "SOUTH", INT2NUM(RPG_SOUTH));
    rb_define_const(dir, "NORTH_WEST", INT2NUM(RPG_NORTH_WEST));
    rb_define_const(dir, "NORTH_EAST", INT2NUM(RPG_NORTH_EAST));
    rb_define_const(dir, "SOUTH_WEST", INT2NUM(RPG_SOUTH_WEST));
    rb_define_const(dir, "SOUTH_EAST", INT2NUM(RPG_SOUTH_EAST));
    rb_define_const(dir, "ALL", INT2NUM(RPG_ALL_DIRECTIONS));
}

static VALUE rpg_empty_method(VALUE obj) {
    return obj;
}