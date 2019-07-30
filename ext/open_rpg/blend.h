#ifndef OPEN_RPG_BLEND_H
#define OPEN_RPG_BLEND_H 1

#include "./common.h"

void rpg_blend_init(VALUE parent);
static VALUE rpg_blend_alloc(VALUE klass);
static VALUE rpg_blend_initialize(VALUE self, VALUE equation, VALUE sfactor, VALUE dfactor);
static VALUE rpg_blend_get_equation(VALUE self);
static VALUE rpg_blend_get_sfactor(VALUE self);
static VALUE rpg_blend_get_dfactor(VALUE self);
static VALUE rpg_blend_set_equation(VALUE self, VALUE value);
static VALUE rpg_blend_set_sfactor(VALUE self, VALUE value);
static VALUE rpg_blend_set_dfactor(VALUE self, VALUE value);
static VALUE rpg_blend_equal(VALUE self, VALUE obj);

static VALUE rpg_blend_normal(VALUE klass);
static VALUE rpg_blend_add(VALUE klass);
static VALUE rpg_blend_subtract(VALUE klass);

#endif /* OPEN_RPG_BLEND_H */