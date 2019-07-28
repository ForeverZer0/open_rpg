#ifndef OPEN_RPG_TONE_H
#define OPEN_RPG_TONE_H 1

#include "common.h"

void rpg_tone_init(VALUE parent);
static VALUE rpg_tone_alloc(VALUE klass);
static VALUE rpg_tone_initialize(int argc, VALUE *argv, VALUE self);
static VALUE rpg_tone_set(int argc, VALUE *argv, VALUE self);
static VALUE rpg_tone_inspect(VALUE self);
static VALUE rpg_tone_dup(VALUE self);
static VALUE rpg_tone_equal(VALUE self, VALUE other);
static VALUE rpg_tone_dump(int argc, VALUE *argv, VALUE self);
static VALUE rpg_tone_load(VALUE klass, VALUE binary);

static VALUE rpg_tone_get_r(VALUE self);
static VALUE rpg_tone_get_g(VALUE self);
static VALUE rpg_tone_get_b(VALUE self);
static VALUE rpg_tone_get_gr(VALUE self);
static VALUE rpg_tone_set_r(VALUE self, VALUE value);
static VALUE rpg_tone_set_g(VALUE self, VALUE value);
static VALUE rpg_tone_set_b(VALUE self, VALUE value);
static VALUE rpg_tone_set_gr(VALUE self, VALUE value);

static VALUE rpg_tone_get_red(VALUE self);
static VALUE rpg_tone_get_green(VALUE self);
static VALUE rpg_tone_get_blue(VALUE self);
static VALUE rpg_tone_get_gray(VALUE self);
static VALUE rpg_tone_set_red(VALUE self, VALUE value);
static VALUE rpg_tone_set_green(VALUE self, VALUE value);
static VALUE rpg_tone_set_blue(VALUE self, VALUE value);
static VALUE rpg_tone_set_gray(VALUE self, VALUE value);

#endif /* OPEN_RPG_TONE_H */