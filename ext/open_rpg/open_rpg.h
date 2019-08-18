#ifndef OPEN_RPG_H
#define OPEN_RPG_H 1

#include "./rpg.h"
#include "./audio.h"
#include "./font.h"
#include "./renderable.h"
#include "./table.h"
#include "./batch.h"

void rpg_color_init(VALUE parent);
void rpg_tone_init(VALUE parent);
void rpg_geometry_init(VALUE parent);
void rpg_numerics_init(VALUE parent);
void rpg_image_init(VALUE parent);
void rpg_shader_init(VALUE parent);
void rpg_app_init(VALUE parent);
void rpg_input_init(VALUE parent);
void rpg_tmx_init(VALUE parent);
void rpg_timespan_init(VALUE parent);

/**
 * @brief Updates the game input, clearing all single-frame states.
 */
void rpg_input_update(void);

/**
 * @brief Creates and returns a new image copied from the back buffer.
 * 
 * @return RPGimage* The created image, must be freed when no longer needed.
 */
RPGimage *rpg_snapshot(void);

/**
 * @brief Renders the game scene to the currently bound framebuffer.
 */
void rpg_render(void);

// Callbacks
void rpg_app_files_dropped(RPGwindow *window, int count, const char **filepaths);
void rpg_app_moved(RPGwindow *window, int x, int y);
void rpg_app_focused(RPGwindow *window, int focused);
void rpg_app_size_change(RPGwindow *window, int width, int height);
void rpg_app_closing(RPGwindow *window);
void rpg_app_minimized(RPGwindow *window, int minimized);
void rpg_app_maximized(RPGwindow *window, int maximized);

void rpg_buffer_resize_cb(RPGwindow *window, int width, int height);

void rpg_input_key_cb(RPGwindow *window, int key, int scancode, int action, int mods);
void rpg_input_mouse_cb(RPGwindow *window, int button, int action, int mods);
void rpg_input_mouse_move_cb(RPGwindow *window, double x, double y);
void rpg_input_mouse_capture_cb(RPGwindow *window, int entered);
void rpg_input_mouse_scroll_cb(RPGwindow* window, double x, double y);
void rpg_input_text_cb(RPGwindow *window, unsigned int);

#endif /* OPEN_RPG_H */
