#ifndef OPEN_RPG_PLANE_H
#define OPEN_RPG_PLANE_H 1

#include "./common.h"
#include "./renderable.h"
#include "./batch.h"

void rpg_plane_render(void *renderable);

void rpg_plane_init(VALUE parent);
static VALUE rpg_plane_alloc(VALUE klass);
static VALUE rpg_plane_initialize(int argc, VALUE *argv, VALUE self);
static VALUE rpg_plane_dispose(int argc, VALUE *argv, VALUE self);

static VALUE rpg_plane_get_x(VALUE self);
static VALUE rpg_plane_get_y(VALUE self);
static VALUE rpg_plane_get_width(VALUE self);
static VALUE rpg_plane_get_height(VALUE self);
static VALUE rpg_plane_get_location(VALUE self);
static VALUE rpg_plane_get_size(VALUE self);
static VALUE rpg_plane_get_rect(VALUE self);

static VALUE rpg_plane_set_x(VALUE self, VALUE value);
static VALUE rpg_plane_set_y(VALUE self, VALUE value);
static VALUE rpg_plane_set_width(VALUE self, VALUE value);
static VALUE rpg_plane_set_height(VALUE self, VALUE value);
static VALUE rpg_plane_set_location(VALUE self, VALUE value);
static VALUE rpg_plane_set_size(VALUE self, VALUE value);
static VALUE rpg_plane_set_rect(VALUE self, VALUE value);

static VALUE rpg_plane_get_src_scale_x(VALUE self);
static VALUE rpg_plane_get_src_scale_y(VALUE self);
static VALUE rpg_plane_set_src_scale_x(VALUE self, VALUE value);
static VALUE rpg_plane_set_src_scale_y(VALUE self, VALUE value);

static VALUE rpg_plane_get_image(VALUE self);
static VALUE rpg_plane_set_image(VALUE self, VALUE value);
static VALUE rpg_plane_set_ox(VALUE self, VALUE value);
static VALUE rpg_plane_set_oy(VALUE self, VALUE value);

#endif /* OPEN_RPG_PLANE_H */