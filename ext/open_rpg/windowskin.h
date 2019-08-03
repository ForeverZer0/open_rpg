#ifndef OPEN_RPG_WINDOWSKIN_H
#define OPEN_RPG_WINDOWSKIN_H 1

#include "./common.h"

void rpg_windowskin_init(VALUE parent);
void rpg_windowskin_free(void *data);
static VALUE rpg_windowskin_alloc(VALUE klass);

static VALUE rpg_windowskin_get_image(VALUE self);
static VALUE rpg_windowskin_set_image(VALUE self, VALUE value);
static VALUE rpg_windowskin_get_frame(VALUE self);
static VALUE rpg_windowskin_set_frame(VALUE self, VALUE value);
static VALUE rpg_windowskin_get_background(VALUE self);
static VALUE rpg_windowskin_set_background(VALUE self, VALUE value);
static VALUE rpg_windowskin_get_overlay(VALUE self);
static VALUE rpg_windowskin_set_overlay(VALUE self, VALUE value);
static VALUE rpg_windowskin_get_selector(VALUE self);
static VALUE rpg_windowskin_set_selector(VALUE self, VALUE value);
static VALUE rpg_windowskin_get_cursors(VALUE self);
static VALUE rpg_windowskin_set_cursors(VALUE self, VALUE a1, VALUE a2, VALUE a3, VALUE a4);
static VALUE rpg_windowskin_get_stretch_frame(VALUE self);
static VALUE rpg_windowskin_set_stretch_frame(VALUE self, VALUE value);
static VALUE rpg_windowskin_get_stretch_background(VALUE self);
static VALUE rpg_windowskin_set_stretch_background(VALUE self, VALUE value);
static VALUE rpg_windowskin_get_stretch_overlay(VALUE self);
static VALUE rpg_windowskin_set_stretch_overlay(VALUE self, VALUE value);
static VALUE rpg_windowskin_get_frame_size(VALUE self);
static VALUE rpg_windowskin_set_frame_size(VALUE self, VALUE value);
static VALUE rpg_windowskin_get_selector_size(VALUE self);
static VALUE rpg_windowskin_set_selector_size(VALUE self, VALUE value);
static VALUE rpg_windowskin_get_arrows(VALUE self);
static VALUE rpg_windowskin_set_arrows(VALUE self, VALUE up, VALUE down, VALUE left, VALUE right);

#endif /* OPEN_RPG_WINDOWSKIN_H */