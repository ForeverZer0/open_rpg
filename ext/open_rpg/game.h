#ifndef OPEN_RPG_GAME_H
#define OPEN_RPG_GAME_H 1

#include "common.h"
#include "input.h"
#include "numerics.h"
#include "sprite.h"

#define STOCK_VERTEX_SHADER "./shaders/stock.vert"
#define STOCK_FRAGMENT_SHADER "./shaders/stock.frag"

void rpg_game_init(VALUE parent);
void rpg_game_window_resize(GLFWwindow *window, int width, int height);
void rpg_game_glfw_error(int code, const char* message);
static GLuint rpg_game_create_shader(void);

static VALUE rpg_game_allocate(VALUE klass);
static VALUE rpg_game_initialize(int argc, VALUE *argv, VALUE self);
static VALUE rpg_game_main(int argc, VALUE *argv, VALUE self);

static VALUE rpg_game_update(VALUE self);
static VALUE rpg_game_close(int argc, VALUE *argv, VALUE self);
static VALUE rpg_game_closing_p(VALUE self);
static VALUE rpg_game_dispose(VALUE self);
static VALUE rpg_game_disposed_p(VALUE self);
static VALUE rpg_game_get_frame_rate(VALUE self);
static VALUE rpg_game_set_frame_rate(VALUE self, VALUE rate);
static VALUE rpg_game_get_frame_count(VALUE self);
static VALUE rpg_game_set_frame_count(VALUE self, VALUE count);
static VALUE rpg_game_get_bg_color(VALUE self);
static VALUE rpg_game_set_bg_color(VALUE self, VALUE value);
static VALUE rpg_game_viewport(VALUE self);

static VALUE rpg_game_window_size(VALUE self);
static VALUE rpg_game_window_width(VALUE self);
static VALUE rpg_game_window_height(VALUE self);

static VALUE rpg_game_resolution(VALUE self);
static VALUE rpg_game_width(VALUE self);
static VALUE rpg_game_height(VALUE self);

static VALUE rpg_game_default_options(VALUE klass);

#endif /* OPEN_RPG_GAME_H */