#ifndef OPEN_RPG_SPRITE_H
#define OPEN_RPG_SPRITE_H 1

#include "./common.h"
#include "./graphics.h"
#include "./renderable.h"

void rpg_sprite_init(VALUE parent);
void rpg_sprite_render(void *sprite);

static VALUE rpg_sprite_alloc(VALUE klass);
static VALUE rpg_sprite_dispose(int argc, VALUE *argv, VALUE self);
static VALUE rpg_sprite_initialize(int argc, VALUE *argv, VALUE self);

static VALUE rpg_sprite_get_x(VALUE self);
static VALUE rpg_sprite_set_x(VALUE self, VALUE value);
static VALUE rpg_sprite_get_y(VALUE self);
static VALUE rpg_sprite_set_y(VALUE self, VALUE value);
static VALUE rpg_sprite_set_z(VALUE self, VALUE value);
static VALUE rpg_sprite_viewport(VALUE self);
static VALUE rpg_sprite_get_image(VALUE self);
static VALUE rpg_sprite_set_image(VALUE self, VALUE value);
static VALUE rpg_sprite_get_rect(VALUE self);
static VALUE rpg_sprite_set_rect(VALUE self, VALUE value);
static VALUE rpg_sprite_get_hue(VALUE self);
static VALUE rpg_sprite_set_hue(VALUE self, VALUE value);

#endif /* OPEN_RPG_SPRITE_H */