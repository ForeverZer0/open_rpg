#ifndef OPEN_RPG_GRAPHICS_H
#define OPEN_RPG_GRAPHICS_H 1

#include "./app.h"
#include "./batch.h"
#include "./common.h"
#include "./input.h"
#include "./numerics.h"
#include "./shader.h"

#define MAX_FRAME_RATE 1000
#define MIN_FRAME_RATE 1
#define DEFAULT_FRAME_RATE 40
#define STOCK_VERTEX_SHADER "stock.vert"
#define STOCK_FRAGMENT_SHADER "stock.frag"

// Internal
void rpg_graphics_init(VALUE parent);
void rpg_graphics_error(int code, const char *message);
void rpg_graphics_buffer_resize(GLFWwindow *window, int width, int height);
void rpg_graphics_resolution(int width, int height);
VALUE rpg_graphics_capture(VALUE module);

static VALUE rpg_game_main(int argc, VALUE *argv, VALUE module);

static VALUE rpg_graphics_create(int argc, VALUE *argv, VALUE module);
static VALUE rpg_graphics_width(VALUE module);
static VALUE rpg_graphics_height(VALUE module);
static VALUE rpg_graphics_get_size(VALUE module);
static VALUE rpg_graphics_set_size(VALUE module, VALUE value);
static VALUE rpg_graphics_get_frame_count(VALUE module);
static VALUE rpg_graphics_get_frame_rate(VALUE module);
static VALUE rpg_graphics_set_frame_count(VALUE module, VALUE value);
static VALUE rpg_graphics_set_frame_rate(VALUE module, VALUE value);
static VALUE rpg_graphics_freeze(VALUE module);
static VALUE rpg_graphics_frozen_p(VALUE module);
static VALUE rpg_graphics_transition(int argc, VALUE *argv, VALUE module);
static VALUE rpg_graphics_get_bg_color(VALUE module);
static VALUE rpg_graphics_set_bg_color(VALUE module, VALUE value);
static VALUE rpg_graphics_get_vsync(VALUE module);
static VALUE rpg_graphics_set_vsync(VALUE module, VALUE value);
static VALUE rpg_graphics_destroy(VALUE module);

#endif /* OPEN_RPG_GRAPHICS_H */