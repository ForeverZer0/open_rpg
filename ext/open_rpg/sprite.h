#ifndef OPEN_RPG_SPRITE_H
#define OPEN_RPG_SPRITE_H 1

#include "common.h"
#include "graphics.h"
#include "batch.h"
#include "numerics.h"

void rpg_sprite_init(VALUE parent);
void rpg_sprite_render(void *sprite);

static VALUE rpg_sprite_alloc(VALUE klass);
static VALUE rpg_sprite_initialize(int argc, VALUE *argv, VALUE self);
static VALUE rpg_sprite_dispose(int argc, VALUE *argv, VALUE self);
static VALUE rpg_sprite_disposed_p(VALUE self);

static VALUE rpg_sprite_get_x(VALUE self);
static VALUE rpg_sprite_set_x(VALUE self, VALUE value);
static VALUE rpg_sprite_get_y(VALUE self);
static VALUE rpg_sprite_set_y(VALUE self, VALUE value);
static VALUE rpg_sprite_set_z(VALUE self, VALUE value);

static VALUE rpg_sprite_get_angle(VALUE self);
static VALUE rpg_sprite_set_angle(VALUE self, VALUE value);
static VALUE rpg_sprite_get_anchor(VALUE self);
static VALUE rpg_sprite_set_anchor(VALUE self, VALUE value);
static VALUE rpg_sprite_get_bitmap(VALUE self);
static VALUE rpg_sprite_set_bitmap(VALUE self, VALUE value);

static VALUE rpg_sprite_get_scale(VALUE self);
static VALUE rpg_sprite_set_scale(VALUE self, VALUE value);
static VALUE rpg_sprite_zoom(VALUE self, VALUE value);
static VALUE rpg_sprite_get_zoom_x(VALUE self);
static VALUE rpg_sprite_set_zoom_x(VALUE self, VALUE value);
static VALUE rpg_sprite_get_zoom_y(VALUE self);
static VALUE rpg_sprite_set_zoom_y(VALUE self, VALUE value);
static VALUE rpg_sprite_viewport(VALUE self);

static VALUE rpg_sprite_get_blend(VALUE self);
static VALUE rpg_sprite_set_blend(VALUE self, VALUE value);

#if RMXP_COMPAT
static VALUE rpg_sprite_get_blend_type(VALUE self);
static VALUE rpg_sprite_set_blend_type(VALUE self, VALUE value);
#endif

#endif /* OPEN_RPG_SPRITE_H */