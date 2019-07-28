#ifndef OPEN_RPG_COLOR_H
#define OPEN_RPG_COLOR_H 1

#include "common.h"

void rpg_color_init(VALUE parent);

static VALUE rpg_color_alloc(VALUE klass);
static VALUE rpg_color_initialize(int argc, VALUE *argv, VALUE self);
static VALUE rpg_color_set(int argc, VALUE *argv, VALUE self);

static VALUE rpg_color_get_r(VALUE self);
static VALUE rpg_color_get_g(VALUE self);
static VALUE rpg_color_get_b(VALUE self);
static VALUE rpg_color_get_a(VALUE self);
static VALUE rpg_color_set_r(VALUE self, VALUE value);
static VALUE rpg_color_set_g(VALUE self, VALUE value);
static VALUE rpg_color_set_b(VALUE self, VALUE value);
static VALUE rpg_color_set_a(VALUE self, VALUE value);

static VALUE rpg_color_get_red(VALUE self);
static VALUE rpg_color_get_green(VALUE self);
static VALUE rpg_color_get_blue(VALUE self);
static VALUE rpg_color_get_alpha(VALUE self);
static VALUE rpg_color_set_red(VALUE self, VALUE value);
static VALUE rpg_color_set_green(VALUE self, VALUE value);
static VALUE rpg_color_set_blue(VALUE self, VALUE value);
static VALUE rpg_color_set_alpha(VALUE self, VALUE value);

static VALUE rpg_color_rgb(VALUE self);
static VALUE rpg_color_rgba(VALUE self);
static VALUE rpg_color_bgr(VALUE self);
static VALUE rpg_color_bgra(VALUE self);
static VALUE rpg_color_hsv(VALUE self);
static VALUE rpg_color_hsl(VALUE self);
static VALUE rpg_color_html(VALUE self);
static VALUE rpg_color_to_i(VALUE self);
static VALUE rpg_color_unpack(VALUE klass, VALUE i);

static VALUE rpg_color_invert(int argc, VALUE *argv, VALUE self);
static VALUE rpg_color_invert_bang(int argc, VALUE *argv, VALUE self);
static VALUE rpg_color_dup(VALUE self);
static VALUE rpg_color_lerp(VALUE self, VALUE other, VALUE amount);
static VALUE rpg_color_lerp_bang(VALUE self, VALUE other, VALUE amount);
static VALUE rpg_color_inspect(VALUE self);
static VALUE rpg_color_equal(VALUE self, VALUE other);

static VALUE rpg_color_dump(int argc, VALUE *argv, VALUE self);
static VALUE rpg_color_load(VALUE klass, VALUE binary);

static VALUE rpg_color_from_hsv(int argc, VALUE *argv, VALUE klass);
static VALUE rpg_color_from_hsl(int argc, VALUE *argv, VALUE klass);

#endif /* OPEN_RPG_COLOR_H */