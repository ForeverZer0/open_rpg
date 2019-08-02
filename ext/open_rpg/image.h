#ifndef OPEN_RPG_IMAGE_H
#define OPEN_RPG_IMAGE_H 1

#define BYTES_PER_PIXEL 4

#include "./common.h"
#include "./font.h"
#include "./renderable.h"
#include "./shader.h"
#include <unistd.h>

enum RPGimage_format { RPG_FORMAT_PNG, RPG_FORMAT_JPG, RPG_FORMAT_BMP };

void rpg_image_init(VALUE parent);
static VALUE rpg_image_alloc(VALUE klass);
void *rpg_image_pixels(RPGimage *image, int *size);
static VALUE rpg_image_initialize(int argc, VALUE *argv, VALUE self);

static VALUE rpg_image_get_font(VALUE self);
static VALUE rpg_image_set_font(VALUE self, VALUE value);
static VALUE rpg_image_draw_text(int argc, VALUE *argv, VALUE self);

static VALUE rpg_image_blob(VALUE self);
static VALUE rpg_image_get_pixel(int argc, VALUE *argv, VALUE self);
static VALUE rpg_image_set_pixel(int argc, VALUE *argv, VALUE self);
static VALUE rpg_image_fill_rect(int argc, VALUE *argv, VALUE self);
static VALUE rpg_image_clear(VALUE self);
static VALUE rpg_image_save(VALUE self, VALUE path, VALUE format);

static VALUE rpg_image_slice(int argc, VALUE *argv, VALUE self);
static VALUE rpg_image_blit(int argc, VALUE *argv, VALUE self);

static VALUE rpg_image_from_blob(int argc, VALUE *argv, VALUE klass);
static VALUE rpg_image_from_file(VALUE klass, VALUE path);

static VALUE rpg_image_fbo(VALUE self);
static VALUE rpg_image_texture(VALUE self);

static VALUE rpg_image_width(VALUE self);
static VALUE rpg_image_height(VALUE self);
static VALUE rpg_image_size(VALUE self);
static VALUE rpg_image_rect(VALUE self);

#endif /* OPEN_RPG_IMAGE_H */