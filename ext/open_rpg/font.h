#ifndef OPEN_RPG_FONT_H
#define OPEN_RPG_FONT_H 1

#include "./rpg.h"
#include FT_FREETYPE_H
#include FT_STROKER_H

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

typedef struct _RPGglyph {
    UT_hash_handle glyph_handle;
    int codepoint;
    GLuint texture;
    RPGsize size;
    RPGpoint bearing;
    GLint advance;
    struct {
        GLuint texture;
        RPGsize size;
        RPGpoint bearing;
        GLint advance;
    } outline;
} RPGglyph;

typedef struct _RPGface_size {
    UT_hash_handle size_handle;
    FT_UInt size;
    int offset;
    RPGglyph *glyphs;
} RPGface_size;

typedef struct _RPGfont_face {
    UT_hash_handle face_handle;
    ID path;
    FT_Face face;
    RPGface_size *sizes;
} RPGfont_face;

void rpg_font_init(VALUE parent);
void rpg_font_render(RPGfont *font, RPGmat4 *ortho, const char *str, int x, int y);
void rpg_font_measure_s(RPGfont *font, void *str, RPGsize *size);

#endif /* OPEN_RPG_FONT_H */