#ifndef OPEN_RPG_APPLICATION_H
#define OPEN_RPG_APPLICATION_H 1

#include "./common.h"
#include "./image.h"

void rpg_app_init(VALUE parent);
void rpg_app_caption(const char *str);

static VALUE rpg_app_get_caption(VALUE module);
static VALUE rpg_app_set_caption(VALUE module, VALUE value);
static VALUE rpg_app_window_size(VALUE module);
static VALUE rpg_app_window_width(VALUE module);
static VALUE rpg_app_window_height(VALUE module);
static VALUE rpg_app_close(int argc, VALUE *argv, VALUE module);
static VALUE rpg_app_closing_p(VALUE module);
static VALUE rpg_app_set_icon(int argc, VALUE *argv, VALUE module);

static VALUE rpg_app_on_file_drop(VALUE module);
static VALUE rpg_app_on_move(VALUE module);
static VALUE rpg_app_on_focus_change(VALUE module);
static VALUE rpg_app_on_size_changed(VALUE module);
static VALUE rpg_app_on_close(VALUE module);
static VALUE rpg_app_on_minimize_change(VALUE module);
static VALUE rpg_app_on_maximize_change(VALUE module);

static VALUE rpg_app_focus(VALUE module);
static VALUE rpg_app_focused_p(VALUE module);
static VALUE rpg_app_minmize(VALUE module);
static VALUE rpg_app_minmized_p(VALUE module);
static VALUE rpg_app_maximize(VALUE module);
static VALUE rpg_app_maximized_p(VALUE module);
static VALUE rpg_app_restore(VALUE module);
static VALUE rpg_app_show(VALUE module);
static VALUE rpg_app_hide(VALUE module);
static VALUE rpg_app_request_attention(VALUE module);
static VALUE rpg_app_move(int argc, VALUE *argv, VALUE module);


// Callbacks
static void rpg_app_files_dropped(GLFWwindow *window, int count, const char **filepaths);
static void rpg_app_moved(GLFWwindow *window, int x, int y);
static void rpg_app_focused(GLFWwindow *window, int focused);
static void rpg_app_size_change(GLFWwindow *window, int width, int height);
static void rpg_app_closing(GLFWwindow *window);
static void rpg_app_minimized(GLFWwindow *window, int minimized);
static void rpg_app_maximized(GLFWwindow *window, int maximized);

#endif /* OPEN_RPG_APPLICATION_H */