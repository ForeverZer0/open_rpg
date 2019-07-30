#ifndef OPEN_RPG_RENDERABLE_H
#define OPEN_RPG_RENDERABLE_H 1

#include "common.h"

void rpg_renderable_init(VALUE parent);
static VALUE rpg_renderable_alloc(VALUE klass);
static VALUE rpg_renderable_initialize(VALUE self);
static VALUE rpg_renderable_dispose(VALUE self);
static VALUE rpg_renderable_disposed_p(VALUE self);
static VALUE rpg_renderable_update(VALUE self);

static VALUE rpg_renderable_get_alpha(VALUE self);
static VALUE rpg_renderable_set_alpha(VALUE self, VALUE value);
static VALUE rpg_renderable_get_opacity(VALUE self);
static VALUE rpg_renderable_set_opacity(VALUE self, VALUE value);
static VALUE rpg_renderable_get_visible(VALUE self);
static VALUE rpg_renderable_set_visible(VALUE self, VALUE value);
static VALUE rpg_renderable_get_origin(VALUE self);
static VALUE rpg_renderable_set_origin(VALUE self, VALUE value);
static VALUE rpg_renderable_get_ox(VALUE self);
static VALUE rpg_renderable_get_oy(VALUE self);
static VALUE rpg_renderable_get_z(VALUE self);
static VALUE rpg_renderable_set_ox(VALUE self, VALUE value);
static VALUE rpg_renderable_set_oy(VALUE self, VALUE value);
static VALUE rpg_renderable_set_z(VALUE self, VALUE value);
static VALUE rpg_renderable_get_color(VALUE self);
static VALUE rpg_renderable_set_color(VALUE self, VALUE value);
static VALUE rpg_renderable_get_tone(VALUE self);
static VALUE rpg_renderable_set_tone(VALUE self, VALUE value);

static VALUE rpg_renderable_flash(VALUE self, VALUE color, VALUE duration);
static VALUE rpg_renderable_flash_duration(VALUE self);
static VALUE rpg_renderable_flash_color(VALUE self);
static VALUE rpg_renderable_flashing_p(VALUE self);

#endif /* OPEN_RPG_RENDERABLE_H */