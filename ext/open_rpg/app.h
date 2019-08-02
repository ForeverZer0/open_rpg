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


// Callbacks
static void rpg_app_files_dropped(GLFWwindow *window, int count, const char **filepaths);

#endif /* OPEN_RPG_APPLICATION_H */