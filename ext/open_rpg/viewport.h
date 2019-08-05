#ifndef OPEN_RPG_VIEWPORT
#define OPEN_RPG_VIEWPORT 1

#include "./batch.h"
#include "./common.h"
#include "./renderable.h"

void rpg_viewport_init(VALUE parent);
VALUE rpg_viewport_alloc(VALUE klass);
static VALUE rpg_viewport_dispose(VALUE self);
static VALUE rpg_viewport_initialize(int argc, VALUE *argv, VALUE self);

static VALUE rpg_viewport_get_x(VALUE self);
static VALUE rpg_viewport_get_y(VALUE self);
static VALUE rpg_viewport_set_x(VALUE self, VALUE value);
static VALUE rpg_viewport_set_y(VALUE self, VALUE value);
static VALUE rpg_viewport_move(int argc, VALUE *argv, VALUE self);
static VALUE rpg_viewport_width(VALUE self);
static VALUE rpg_viewport_height(VALUE self);

static VALUE rpg_viewport_rect(VALUE self);
static VALUE rpg_viewport_location(VALUE self);
static VALUE rpg_viewport_size(VALUE self);
static VALUE rpg_viewport_set_z(VALUE self, VALUE value);
static VALUE rpg_viewport_inspect(VALUE self);

void rpg_viewport_render(void *viewport);

#endif /* OPEN_RPG_VIEWPORT */