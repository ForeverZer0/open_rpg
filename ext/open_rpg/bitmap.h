#ifndef OPEN_RPG_IMAGE_H
#define OPEN_RPG_IMAGE_H 1

#define BYTES_PER_PIXEL 4

#include "common.h"
#include "font.h"
#include "numerics.h"

enum RPGbitmap_format {
    RPG_FORMAT_PNG,
    RPG_FORMAT_JPG,
    RPG_FORMAT_BMP
};

void rpg_bitmap_init(VALUE parent);
static VALUE rpg_bitmap_alloc(VALUE klass);
void rpg_bitmap_free(void *data);
void* rpg_bitmap_load(const char *fname, int *width, int *height);
void* rpg_bitmap_pixels(RPGbitmap *bitmap, int *size);

void rpg_bitmap_bind_fbo(RPGbitmap *img, int x, int y, int width, int height);
void rpg_bitmap_unbind_fbo(void);

static VALUE rpg_bitmap_initialize(int argc, VALUE *argv, VALUE self);
static VALUE rpg_bitmap_dispose(VALUE self);
static VALUE rpg_bitmap_disposed_p(VALUE self);

static VALUE rpg_bitmap_get_font(VALUE self);
static VALUE rpg_bitmap_set_font(VALUE self, VALUE value);
static VALUE rpg_bitmap_draw_text(int argc, VALUE *argv, VALUE self);

static VALUE rpg_bitmap_blob(VALUE self);
static VALUE rpg_bitmap_get_pixel(int argc, VALUE *argv, VALUE self);
static VALUE rpg_bitmap_set_pixel(int argc, VALUE *argv, VALUE self);
static VALUE rpg_bitmap_fill_rect(int argc, VALUE *argv, VALUE self);
static VALUE rpg_bitmap_blit(int argc, VALUE *argv, VALUE self);
static VALUE rpg_bitmap_slice(int argc, VALUE *argv, VALUE self);
static VALUE rpg_bitmap_clear(VALUE self);
static VALUE rpg_bitmap_save(VALUE self, VALUE path, VALUE format);

static VALUE rpg_bitmap_from_blob(int argc, VALUE *argv, VALUE klass);

static VALUE rpg_bitmap_width(VALUE self);
static VALUE rpg_bitmap_height(VALUE self);
static VALUE rpg_bitmap_size(VALUE self);
static VALUE rpg_bitmap_rect(VALUE self);

#endif /* OPEN_RPG_IMAGE_H */