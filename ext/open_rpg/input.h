#ifndef OPEN_RPG_INPUT_H
#define OPEN_RPG_INPUT_H 1

#include "./common.h"
#include "./image.h"

void rpg_input_init(VALUE parent);
VALUE rpg_input_update(VALUE module);

// Callbacks
void rpg_input_key_cb(GLFWwindow *window, int key, int scancode, int action, int mods);
void rpg_input_mouse_cb(GLFWwindow *window, int button, int action, int mods);
void rpg_input_mouse_move_cb(GLFWwindow *window, double x, double y);

// Keyboard
static VALUE rpg_kb_trigger_p(VALUE module, VALUE key);
static VALUE rpg_kb_repeat_p(VALUE module, VALUE key);
static VALUE rpg_kb_release_p(VALUE module, VALUE key);
static VALUE rpg_kb_press_p(VALUE module, VALUE key);
static VALUE rpg_kb_key_name(VALUE module, VALUE key);
static VALUE rpg_kb_scancode_name(VALUE module, VALUE scancode);
static VALUE rpg_kb_key_scancode(VALUE module, VALUE key);

// Mouse
static VALUE rpg_mouse_trigger_p(VALUE module, VALUE key);
static VALUE rpg_mouse_repeat_p(VALUE module, VALUE key);
static VALUE rpg_mouse_release_p(VALUE module, VALUE key);
static VALUE rpg_mouse_press_p(VALUE module, VALUE key);
static VALUE rpg_mouse_change_cursor(int argc, VALUE *argv, VALUE module);
static VALUE rpg_mouse_location(VALUE module);
static VALUE rpg_mouse_x(VALUE module);
static VALUE rpg_mouse_y(VALUE module);

static VALUE rpg_input_on_mouse_move(VALUE module);
static VALUE rpg_input_on_key(VALUE module);

#define KEY_FIRST GLFW_KEY_SPACE
#define KEY_LAST GLFW_KEY_LAST

#define INPUT_STATE_NONE 0
#define INPUT_STATE_RELEASE 1
#define INPUT_STATE_TRIGGER 2
#define INPUT_STATE_REPEAT 3
#define INPUT_STATE_PRESS 4

#endif /* OPEN_RPG_INPUT_H */