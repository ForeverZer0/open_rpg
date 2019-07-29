#ifndef OPEN_RPG_SPRITE_H
#define OPEN_RPG_SPRITE_H 1

#include "common.h"
#include "numerics.h"

void rpg_sprite_init(VALUE parent);
static VALUE rpg_sprite_alloc(VALUE klass);
static VALUE rpg_sprite_dispose(int argc, VALUE *argv, VALUE self);
static VALUE rpg_sprite_disposed_p(VALUE self);

void rpg_sprite_render(RPGsprite *sprite);

static VALUE rpg_sprite_initialize(int argc, VALUE *argv, VALUE self);
static VALUE rpg_sprite_update(VALUE self);

static VALUE rpg_sprite_get_x(VALUE self);
static VALUE rpg_sprite_set_x(VALUE self, VALUE value);
static VALUE rpg_sprite_get_y(VALUE self);
static VALUE rpg_sprite_set_y(VALUE self, VALUE value);
static VALUE rpg_sprite_get_z(VALUE self);
static VALUE rpg_sprite_set_z(VALUE self, VALUE value);

static VALUE rpg_sprite_get_angle(VALUE self);
static VALUE rpg_sprite_set_angle(VALUE self, VALUE value);
static VALUE rpg_sprite_get_anchor(VALUE self);
static VALUE rpg_sprite_set_anchor(VALUE self, VALUE value);
static VALUE rpg_sprite_get_image(VALUE self);
static VALUE rpg_sprite_set_image(VALUE self, VALUE value);

static VALUE rpg_sprite_get_scale(VALUE self);
static VALUE rpg_sprite_set_scale(VALUE self, VALUE value);
static VALUE rpg_sprite_get_scale_x(VALUE self);
static VALUE rpg_sprite_set_scale_x(VALUE self, VALUE value);
static VALUE rpg_sprite_get_scale_y(VALUE self);
static VALUE rpg_sprite_set_scale_y(VALUE self, VALUE value);

#endif /* OPEN_RPG_SPRITE_H */