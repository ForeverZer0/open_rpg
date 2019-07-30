#ifndef OPEN_RPG_VIEWPORT
#define OPEN_RPG_VIEWPORT 1

#include "./common.h"
#include "./batch.h"
#include "./numerics.h"

void rpg_viewport_init(VALUE parent);
static VALUE rpg_viewport_alloc(VALUE klass);
static VALUE rpg_viewport_initialize(int argc, VALUE *argv, VALUE self);
static VALUE rpg_viewport_dispose(VALUE self);
static VALUE rpg_viewport_disposed_p(VALUE self);

static VALUE rpg_viewport_rect(VALUE self);
static VALUE rpg_viewport_location(VALUE self);
static VALUE rpg_viewport_size(VALUE self);
static VALUE rpg_viewport_set_z(VALUE self, VALUE value);

void rpg_viewport_render(void *viewport);

// TODO: ==, inspect

#endif /* OPEN_RPG_VIEWPORT */