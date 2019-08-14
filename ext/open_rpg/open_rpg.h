#ifndef OPEN_RPG_H
#define OPEN_RPG_H 1

#include "./common.h"
#include "./font.h"
#include "./renderable.h"
#include "./table.h"
#include "./tilemap.h"
#include "./batch.h"

void rpg_color_init(VALUE parent);
void rpg_tone_init(VALUE parent);
void rpg_geometry_init(VALUE parent);
void rpg_numerics_init(VALUE parent);
void rpg_image_init(VALUE parent);
void rpg_shader_init(VALUE parent);
void rpg_app_init(VALUE parent);
void rpg_input_init(VALUE parent);


VALUE rpg_input_update(VALUE module);


// Callbacks
void rpg_app_files_dropped(GLFWwindow *window, int count, const char **filepaths);
void rpg_app_moved(GLFWwindow *window, int x, int y);
void rpg_app_focused(GLFWwindow *window, int focused);
void rpg_app_size_change(GLFWwindow *window, int width, int height);
void rpg_app_closing(GLFWwindow *window);
void rpg_app_minimized(GLFWwindow *window, int minimized);
void rpg_app_maximized(GLFWwindow *window, int maximized);

void rpg_graphics_init(VALUE parent);
void rpg_graphics_error(int code, const char *message);
void rpg_graphics_buffer_resize(GLFWwindow *window, int width, int height);
void rpg_graphics_resolution(int width, int height);

void rpg_input_key_cb(GLFWwindow *window, int key, int scancode, int action, int mods);
void rpg_input_mouse_cb(GLFWwindow *window, int button, int action, int mods);
void rpg_input_mouse_move_cb(GLFWwindow *window, double x, double y);
void rpg_input_mouse_capture_cb(GLFWwindow *window, int entered);
void rpg_input_mouse_scroll_cb(GLFWwindow* window, double x, double y);
void rpg_input_text_cb(GLFWwindow *window, unsigned int);

#endif /* OPEN_RPG_H */
