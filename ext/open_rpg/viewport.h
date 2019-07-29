#ifndef OPEN_RPG_VIEWPORT
#define OPEN_RPG_VIEWPORT 1

#include "common.h"

void rpg_viewport_init(VALUE parent);
static VALUE rpg_viewport_alloc(VALUE klass);
static VALUE rpg_viewport_initialize(int argc, VALUE *argv, VALUE self);
static VALUE rpg_viewport_dispose(VALUE self);
static VALUE rpg_viewport_disposed_p(VALUE self);

static VALUE rpg_viewport_rect(VALUE self);
static VALUE rpg_viewport_location(VALUE self);
static VALUE rpg_viewport_size(VALUE self);



// TODO: ==, inspect, color, tone, flash






#endif /* OPEN_RPG_VIEWPORT */