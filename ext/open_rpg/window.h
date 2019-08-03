#ifndef OPEN_RGP_WINDOW_H
#define OPEN_RGP_WINDOW_H 1

#include "./common.h"
#include "./renderable.h"

void rpg_window_init(VALUE parent);
static VALUE rpg_window_alloc(VALUE klass);
static VALUE rpg_window_initialize(VALUE self, VALUE x, VALUE y, VALUE width, VALUE height);

static VALUE rpg_window_get_x(VALUE self);
static VALUE rpg_window_get_y(VALUE self);
static VALUE rpg_window_get_width(VALUE self);
static VALUE rpg_window_get_height(VALUE self);
static VALUE rpg_window_get_size(VALUE self);
static VALUE rpg_window_get_location(VALUE self);
static VALUE rpg_window_get_rect(VALUE self);
static VALUE rpg_window_set_x(VALUE self, VALUE value);
static VALUE rpg_window_set_y(VALUE self, VALUE value);
static VALUE rpg_window_set_width(VALUE self, VALUE value);
static VALUE rpg_window_set_height(VALUE self, VALUE value);
static VALUE rpg_window_set_size(VALUE self,  VALUE value);
static VALUE rpg_window_set_location(VALUE self,  VALUE value);
static VALUE rpg_window_set_rect(VALUE self,  VALUE value);
static VALUE rpg_window_move(VALUE self, VALUE x, VALUE y, VALUE width, VALUE height);

static VALUE rpg_window_get_windowskin(VALUE self);
static VALUE rpg_window_set_windowskin(VALUE self, VALUE value);
static VALUE rpg_window_get_contents(VALUE self);
static VALUE rpg_window_set_contents(VALUE self, VALUE value);

static VALUE rpg_window_dispose(VALUE self);
static VALUE rpg_window_disposed_p(VALUE self);

#endif /* OPEN_RGP_WINDOW_H */