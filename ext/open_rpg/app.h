#ifndef OPEN_RPG_APPLICATION_H
#define OPEN_RPG_APPLICATION_H 1

#include "./common.h"
#include "./image.h"

void rpg_app_init(VALUE parent);

// Callbacks
static void rpg_app_files_dropped(GLFWwindow *window, int count, const char **filepaths);
static void rpg_app_moved(GLFWwindow *window, int x, int y);
static void rpg_app_focused(GLFWwindow *window, int focused);
static void rpg_app_size_change(GLFWwindow *window, int width, int height);
static void rpg_app_closing(GLFWwindow *window);
static void rpg_app_minimized(GLFWwindow *window, int minimized);
static void rpg_app_maximized(GLFWwindow *window, int maximized);

#endif /* OPEN_RPG_APPLICATION_H */