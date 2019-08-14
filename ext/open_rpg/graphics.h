#ifndef OPEN_RPG_GRAPHICS_H
#define OPEN_RPG_GRAPHICS_H 1

#include "./app.h"
#include "./batch.h"
#include "./common.h"
#include "./input.h"
#include "./shader.h"

#define MAX_FRAME_RATE 1000
#define MIN_FRAME_RATE 1
#define DEFAULT_FRAME_RATE 40
#define STOCK_VERTEX_SHADER "stock.vert"
#define STOCK_FRAGMENT_SHADER "stock.frag"

static RPGimage *rpg_graphics_snap(void);

// Internal
void rpg_graphics_init(VALUE parent);
void rpg_graphics_error(int code, const char *message);
void rpg_graphics_buffer_resize(GLFWwindow *window, int width, int height);
void rpg_graphics_resolution(int width, int height);




#endif /* OPEN_RPG_GRAPHICS_H */