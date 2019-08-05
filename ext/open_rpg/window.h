#ifndef OPEN_RGP_WINDOW_H
#define OPEN_RGP_WINDOW_H 1

#include "./common.h"
#include "./renderable.h"
#include "./viewport.h"

typedef RPGviewport RPGwindow;

// typedef struct RPGwindow {
//     RPGrenderable base;
//     RPGrect rect;
// } RPGwindow;

void rpg_window_init(VALUE parent);

// static VALUE rpg_window_alloc(VALUE klass);
// static VALUE rpg_window_initialize(VALUE self, VALUE x, VALUE y, VALUE width, VALUE height);

static VALUE rpg_window_get_x(VALUE self);
static VALUE rpg_window_get_y(VALUE self);
static VALUE rpg_window_set_x(VALUE self, VALUE value);
static VALUE rpg_window_set_y(VALUE self, VALUE value);
static VALUE rpg_window_move(int argc, VALUE *argv, VALUE self);
static VALUE rpg_window_width(VALUE self);
static VALUE rpg_window_height(VALUE self);
// static VALUE rpg_window_size(VALUE self);
// static VALUE rpg_window_location(VALUE self);
// static VALUE rpg_window_rect(VALUE self);

#endif /* OPEN_RGP_WINDOW_H */