#ifndef OPEN_RPG_SCREEN_H
#define OPEN_RPG_SCREEN_H 1

#include "common.h"

void rpg_screen_init(VALUE parent);
static VALUE rpg_screen_get_tone(VALUE module);
static VALUE rpg_screen_set_tone(VALUE module, VALUE value);
static VALUE rpg_screen_get_z(VALUE module);
static VALUE rpg_screen_set_z(VALUE module, VALUE value);
static VALUE rpg_screen_update(VALUE module);
static VALUE rpg_screen_transition(VALUE module, VALUE tone, VALUE frames, VALUE num_steps);
static VALUE rpg_screen_reset(VALUE module);

#endif /* OPEN_RPG_SCREEN_H */