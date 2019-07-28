#ifndef OPEN_RPG_FONT_H
#define OPEN_RPG_FONT_H 1

#define uthash_malloc xmalloc
#define uthash_free(ptr,sz) xfree(ptr)

#include "common.h"
#include "utf8.h"
#include "uthash.h"
#include "numerics.h"
#include "shader.h"
#include "image.h"
#include <freetype2/ft2build.h>
#include FT_FREETYPE_H

typedef struct RPGglyph {
    int codepoint;
    GLuint texture;
    RPGsize size;
    RPGpoint bearing;
    GLint advance;
    UT_hash_handle hh;
} RPGglyph;

typedef struct RPGfont {
    FT_UInt pixel_size;
    RPGcolor *color;
    FT_Face face;
    int v_offset;
    RPGglyph *glyphs;
} RPGfont;

enum RPGalignment {
    RPG_ALIGN_TOP_LEFT,
    RPG_ALIGN_TOP_RIGHT,
    RPG_ALIGN_TOP_CENTER,
    RPG_ALIGN_BOTTOM_LEFT,
    RPG_ALIGN_BOTTOM_RIGHT,
    RPG_ALIGN_BOTTOM_CENTER,
    RPG_ALIGN_CENTER_LEFT,
    RPG_ALIGN_CENTER_RIGHT,
    RPG_ALIGN_CENTER,
    RPG_ALIGN_DEFAULT = RPG_ALIGN_CENTER_LEFT
};

void rpg_font_init(VALUE parent);
void rpg_font_terminate(void);
void rpg_font_render(RPGfont *font, const char *str, int x, int y, int width, int height);

static VALUE rpg_font_initialize(VALUE self, VALUE path, VALUE px_size);
static VALUE rpg_font_dispose(VALUE self);
static VALUE rpg_font_size(VALUE self);
static VALUE rpg_font_name(VALUE self);
static VALUE rpg_font_bold(VALUE self);
static VALUE rpg_font_italic(VALUE self);
static VALUE rpg_font_measure(VALUE self, VALUE text);
static VALUE rpg_font_alloc(VALUE klass);
static VALUE rpg_font_draw(int argc, VALUE *argv, VALUE self);
static VALUE rpg_font_get_color(VALUE self);
static VALUE rpg_font_set_color(VALUE self, VALUE value);

static VALUE rpg_font_get_default_color(VALUE klass);
static VALUE rpg_font_set_default_color(VALUE klass, VALUE value);
static VALUE rpg_font_get_default_size(VALUE klass);
static VALUE rpg_font_set_default_size(VALUE klass, VALUE value);

#endif /* OPEN_RPG_FONT_H */