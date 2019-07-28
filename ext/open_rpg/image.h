#ifndef OPEN_RPG_IMAGE_H
#define OPEN_RPG_IMAGE_H 1

#define BYTES_PER_PIXEL 4

#include "common.h"

void rpg_image_init(VALUE parent);
VALUE rpg_image_alloc(VALUE klass);
void rpg_image_free(void *data);
void* rpg_image_load(const char *fname, int *width, int *height);

void rpg_image_bind_fbo(RPGimage *img, int x, int y, int width, int height);
void rpg_image_unbind_fbo(void);

static VALUE rpg_image_initialize(int argc, VALUE *argv, VALUE self);
static VALUE rpg_image_dispose(VALUE self);
static VALUE rpg_image_disposed_p(VALUE self);

static VALUE rpg_image_get_pixel(int argc, VALUE *argv, VALUE self);
static VALUE rpg_image_set_pixel(int argc, VALUE *argv, VALUE self);
static VALUE rpg_image_fill(int argc, VALUE *argv, VALUE self);
static VALUE rpg_image_clear(VALUE self);

static VALUE rpg_image_width(VALUE self);
static VALUE rpg_image_height(VALUE self);
static VALUE rpg_image_size(VALUE self);
static VALUE rpg_image_rect(VALUE self);

#endif /* OPEN_RPG_IMAGE_H */