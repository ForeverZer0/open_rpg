#ifndef OPEN_RPG_FONT_H
#define OPEN_RPG_FONT_H 1

#define DEFAULT_FONT_PATH "./fonts/NotoSans-Bold.ttf"
#define DEFAULT_FONT_SIZE 24

#include "./common.h"
#include "./numerics.h"
#include "./shader.h"
#include "./utf8.h"
#include FT_FREETYPE_H

enum RPGalignment {
    RPG_ALIGN_NONE = 0x00,
    RPG_ALIGN_TOP = 0x01,
    RPG_ALIGN_CENTER_V = 0x02,
    RPG_ALIGN_BOTTOM = 0x04,
    RPG_ALIGN_LEFT = 0x08,
    RPG_ALIGN_CENTER_H = 0x10,
    RPG_ALIGN_RIGHT = 0x20,
    RPG_ALIGN_TOP_LEFT = RPG_ALIGN_TOP | RPG_ALIGN_LEFT,
    RPG_ALIGN_TOP_RIGHT = RPG_ALIGN_TOP | RPG_ALIGN_RIGHT,
    RPG_ALIGN_TOP_CENTER = RPG_ALIGN_TOP | RPG_ALIGN_CENTER_H,
    RPG_ALIGN_BOTTOM_LEFT = RPG_ALIGN_BOTTOM | RPG_ALIGN_LEFT,
    RPG_ALIGN_BOTTOM_RIGHT = RPG_ALIGN_BOTTOM | RPG_ALIGN_RIGHT,
    RPG_ALIGN_BOTTOM_CENTER = RPG_ALIGN_BOTTOM | RPG_ALIGN_CENTER_H,
    RPG_ALIGN_CENTER_LEFT = RPG_ALIGN_CENTER_V | RPG_ALIGN_LEFT,
    RPG_ALIGN_CENTER_RIGHT = RPG_ALIGN_CENTER_V | RPG_ALIGN_RIGHT,
    RPG_ALIGN_CENTER = RPG_ALIGN_CENTER_V | RPG_ALIGN_CENTER_H,
    RPG_ALIGN_DEFAULT = RPG_ALIGN_CENTER_LEFT
};

typedef struct RPGglyph {
    UT_hash_handle glyph_handle;
    int codepoint;
    GLuint texture;
    RPGsize size;
    RPGpoint bearing;
    GLint advance;
} RPGglyph;

typedef struct RPGface_size {
    UT_hash_handle size_handle;
    FT_UInt size;
    int offset;
    RPGglyph *glyphs; 
} RPGface_size;

typedef struct RPGfont_face {
    UT_hash_handle face_handle;
    ID path;
    FT_Face face;
    RPGface_size *sizes;
} RPGfont_face;

void rpg_font_init(VALUE parent);
void rpg_font_free(void *data);
void rpg_font_terminate(void);
void rpg_font_render(RPGfont *font, RPGmatrix4x4 *ortho, const char *str, int x, int y);
void rpg_font_measure_s(RPGfont *font, void *str, RPGsize *size);
static void rpg_font_create_default(void);

static VALUE rpg_font_from_file(int argc, VALUE *argv, VALUE self);
static VALUE rpg_font_get_size(VALUE self);
static VALUE rpg_font_set_size(VALUE self, VALUE value);

static VALUE rpg_font_name(VALUE self);
static VALUE rpg_font_bold(VALUE self);
static VALUE rpg_font_italic(VALUE self);
static VALUE rpg_font_measure(VALUE self, VALUE text);
static VALUE rpg_font_alloc(VALUE klass);
static VALUE rpg_font_get_color(VALUE self);
static VALUE rpg_font_set_color(VALUE self, VALUE value);
static VALUE rpg_font_glyph(VALUE self, VALUE codepoint);
static VALUE rpg_font_each_glyph(VALUE self, VALUE str);

static VALUE rpg_glyph_codepoint(VALUE self);
static VALUE rpg_glyph_texture(VALUE self);
static VALUE rpg_glyph_size(VALUE self);
static VALUE rpg_glyph_bearing(VALUE self);
static VALUE rpg_glyph_advance(VALUE self);
static VALUE rpg_glyph_inspect(VALUE self);

static VALUE rpg_font_get_default(VALUE klass);
static VALUE rpg_font_set_default(VALUE klass, VALUE value);

#endif /* OPEN_RPG_FONT_H */