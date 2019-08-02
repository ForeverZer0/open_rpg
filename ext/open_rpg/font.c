#include "./font.h"

VALUE rb_cFont;

GLuint _font_program;
GLint _font_projection;
GLint _font_color;
GLuint font_vao;
GLuint font_vbo;

FT_Library ft_lib;
RPGfont_face *faces;

RPGfont default_font;

#define VERTEX_SHADER "./shaders/font.vert"
#define FRAGMENT_SHADER "./shaders/font.frag"

void rpg_font_init(VALUE parent) {

    if (FT_Init_FreeType(&ft_lib)) {
        rb_raise(rb_eRPGError, "failed to load FreeType library");
    }

    faces = NULL;
    font_vao = 0;
    font_vbo = 0;
    default_font.color.r = 1.0f;
    default_font.color.g = 1.0f;
    default_font.color.b = 1.0f;
    default_font.color.a = 1.0f;
    default_font.size = DEFAULT_FONT_SIZE;

    rb_cFont = rb_define_class_under(parent, "Font", rb_cObject);
    rb_define_alloc_func(rb_cFont, rpg_font_alloc);

    rb_define_method(rb_cFont, "initialize", rpg_font_initialize, -1);
    rb_define_method(rb_cFont, "measure", rpg_font_measure, 1);
    rb_define_method(rb_cFont, "name", rpg_font_name, 0);
    rb_define_method(rb_cFont, "size", rpg_font_get_size, 0);
    rb_define_method(rb_cFont, "size=", rpg_font_set_size, 1);
    rb_define_method(rb_cFont, "color", rpg_font_get_color, 0);
    rb_define_method(rb_cFont, "color=", rpg_font_set_color, 1);
    rb_define_method(rb_cFont, "bold", rpg_font_bold, 0);
    rb_define_method(rb_cFont, "italic", rpg_font_italic, 0);
}

ALLOC_FUNC(rpg_font_alloc, RPGfont)

static inline FT_Face rpg_font_face(RPGfont *font) {
    RPGfont_face *ff = NULL;
    HASH_FIND(face_handle, faces, &font->path, sizeof(ID), ff);
    if (ff == NULL) {
        rb_raise(rb_eRPGError, "disposed font");
    }
    return ff->face;
}

static VALUE rpg_font_initialize(int argc, VALUE *argv, VALUE self) {

    VALUE path, sz, color;
    rb_scan_args(argc, argv, "12", &path, &sz, &color);

    RPGfont *f = DATA_PTR(self);
    VALUE absolute = rb_file_s_absolute_path(1, &path);
    char *fname = StringValueCStr(absolute);
    ID id = rb_intern(fname);
    FT_UInt size = NIL_P(sz) ? default_font.size : NUM2UINT(sz);
    if (size == 0) {
        rb_raise(rb_eArgError, "size must be greater than 0");
    }

    // Initialize this face if not found
    RPGfont_face *ff = NULL;
    HASH_FIND(face_handle, faces, &id, sizeof(ID), ff);
    if (ff == NULL) {
        ff = ALLOC(RPGfont_face);
        ff->path = id;
        ff->sizes = NULL;
        if (FT_New_Face(ft_lib, fname, 0, &ff->face)) {
            rb_raise(rb_eRPGError, "failed to load font");
        }
        HASH_ADD(face_handle, faces, path, sizeof(ID), ff);
    }

    // Initialize size for this face if not found
    RPGface_size *face_size = NULL;
    HASH_FIND(size_handle, ff->sizes, &size, sizeof(FT_UInt), face_size);
    if (face_size == NULL) {
        face_size = ALLOC(RPGface_size);
        face_size->size = size;
        face_size->glyphs = NULL;

        // Set the offset, using a character that (typically) extends from baseline to peak height
        FT_Set_Pixel_Sizes(ff->face, 0, size);
        FT_Load_Char(ff->face, 'H', FT_LOAD_RENDER);
        face_size->offset = ff->face->glyph->bitmap_top;
        HASH_ADD(size_handle, ff->sizes, size, sizeof(FT_UInt), face_size);
    } 

    if (!font_vao) {
        glGenVertexArrays(1, &font_vao);
        glGenBuffers(1, &font_vbo);
        glBindVertexArray(font_vao);
        glBindBuffer(GL_ARRAY_BUFFER, font_vbo);
        glBufferData(GL_ARRAY_BUFFER, VERTICES_SIZE, NULL, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), NULL);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    if (!_font_program) {
        _font_program = rpg_create_shader_program(VERTEX_SHADER, FRAGMENT_SHADER, NULL);
        _font_projection = glGetUniformLocation(_font_program, "projection");
        _font_color = glGetUniformLocation(_font_program, "color");
    }

    f->path = id;
    f->size = size;
    if (NIL_P(color)) {
        memcpy(&f->color, &default_font.color, sizeof(RPGcolor));
    } else {
        RPGcolor *c = DATA_PTR(color);
        memcpy(&f->color, c, sizeof(RPGcolor));
    }
    return Qnil;
}

static VALUE rpg_font_get_size(VALUE self) {
    RPGfont *font = DATA_PTR(self);
    return UINT2NUM(font->size);
}

static VALUE rpg_font_set_size(VALUE self, VALUE value) {
    RPGfont *font = DATA_PTR(self);
    FT_UInt sz = NUM2UINT(value);
    if (sz == 0) {
        rb_raise(rb_eArgError, "size must be greater than 0");
    }

    RPGfont_face *ff = NULL;
    HASH_FIND(face_handle, faces, &font->path, sizeof(ID), ff);
    if (ff == NULL) {
        rb_raise(rb_eRPGError, "disposed font");
    }

    RPGface_size *face_size = NULL;
    HASH_FIND(size_handle, ff->sizes, &sz, sizeof(FT_UInt), face_size);
    if (face_size == NULL) {
        face_size = ALLOC(RPGface_size);
        face_size->size = sz;
        face_size->glyphs = NULL;
        HASH_ADD(size_handle, ff->sizes, size, sizeof(FT_UInt), face_size);
    }

    font->size = sz;
    return value;
}

static inline RPGglyph *rpg_font_glyph_inline(RPGface_size *face_size, int codepoint, FT_Face f) {
    RPGglyph *g = NULL;
    HASH_FIND(glyph_handle, face_size->glyphs, &codepoint, sizeof(int), g);
    if (g == NULL) {
        g = ALLOC(RPGglyph);
        g->codepoint = codepoint;
        
        FT_Load_Char(f, codepoint, FT_LOAD_RENDER);
        g->size.width = f->glyph->bitmap.width;
        g->size.height = f->glyph->bitmap.rows;

        glGenTextures(1, &g->texture);
        glBindTexture(GL_TEXTURE_2D, g->texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, g->size.width, g->size.height, 0, GL_RED, GL_UNSIGNED_BYTE, f->glyph->bitmap.buffer);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);

        g->bearing.x = f->glyph->bitmap_left;
        g->bearing.y = f->glyph->bitmap_top;
        g->advance = (GLint)f->glyph->advance.x;

        HASH_ADD(glyph_handle, face_size->glyphs, codepoint, sizeof(int), g);
    }
    return g;
}

void rpg_font_render(RPGfont *font, RPGmatrix4x4 *ortho, const char *text, int x, int y) {
    
    if (font == NULL) {
        *font = default_font;
    }

    glUseProgram(_font_program);
    glUniformMatrix4fv(_font_projection, 1, GL_FALSE, (float *)ortho);

    RPGfont_face *ff = NULL;
    HASH_FIND(face_handle, faces, &font->path, sizeof(ID), ff);
    if (ff == NULL) {
        rb_raise(rb_eRPGError, "disposed font");
    }

    RPGface_size *face_size = NULL;
    HASH_FIND(size_handle, ff->sizes, &font->size, sizeof(FT_UInt), face_size);
    FT_Set_Pixel_Sizes(ff->face, 0, font->size);
    if (face_size == NULL) {
        rb_raise(rb_eRPGError, "font not loaded");
    }

    glUniform4f(_font_color, font->color.r, font->color.g, font->color.b, font->color.a);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(font_vao);

    size_t len = utf8len(text);
    void *str = (void *)text;
    utf8_int32_t cp;
    RPGglyph *ch;
    float ox = (float)x;
    for (size_t i = 0; i < len; i++) {

        str = utf8codepoint(str, &cp);
        ch = rpg_font_glyph_inline(face_size, cp, ff->face);
        float xPos = ox + ch->bearing.x;
        float yPos = y + (face_size->offset - ch->bearing.y);
        float w = ch->size.width;
        float h = ch->size.height;
        glBindTexture(GL_TEXTURE_2D, ch->texture);
        glBindBuffer(GL_ARRAY_BUFFER, font_vbo);

        float vertices[VERTICES_COUNT] = {xPos, yPos + h, 0.0f, 1.0f, xPos + w, yPos,     1.0f, 0.0f, xPos,     yPos, 0.0f, 0.0f,
                                         xPos, yPos + h, 0.0f, 1.0f, xPos + w, yPos + h, 1.0f, 1.0f, xPos + w, yPos, 1.0f, 0.0f};

        glBufferSubData(GL_ARRAY_BUFFER, 0, VERTICES_SIZE, vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        ox += ch->advance >> 6;
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

static VALUE rpg_font_bold(VALUE self) {
    RPGfont *font = DATA_PTR(self);
    FT_Face face = rpg_font_face(font);
    return RB_BOOL((face->style_flags & FT_STYLE_FLAG_BOLD) != 0);
}

static VALUE rpg_font_italic(VALUE self) {
    RPGfont *font = DATA_PTR(self);
    FT_Face face = rpg_font_face(font);
    return RB_BOOL((face->style_flags & FT_STYLE_FLAG_ITALIC) != 0);
}

static VALUE rpg_font_get_color(VALUE self) {
    RPGfont *font = DATA_PTR(self);
    RPGcolor *color = ALLOC(RPGcolor);
    memcpy(color, &font->color, sizeof(RPGcolor));
    return Data_Wrap_Struct(rb_cColor, NULL, RUBY_DEFAULT_FREE, color);
}

static VALUE rpg_font_set_color(VALUE self, VALUE value) {
    RPGfont *font = DATA_PTR(self);
    if (NIL_P(value)) {
        memcpy(&font->color, &default_font.color, sizeof(RPGcolor));
    } else {
        RPGcolor *color = DATA_PTR(value);
        memcpy(&font->color, color, sizeof(RPGcolor));
    }
    return value;
}

static VALUE rpg_font_name(VALUE self) {
    RPGfont *font = DATA_PTR(self);
    FT_Face face = rpg_font_face(font);
    const char *name = face->family_name;
    return rb_str_new_cstr(name);
}

static VALUE rpg_font_measure(VALUE self, VALUE text) {
    RPGfont *font = DATA_PTR(self);
    void *str = StringValueCStr(text);
    RPGsize *size = ALLOC(RPGsize);
    rpg_font_measure_s(font, str, size);
    return Data_Wrap_Struct(rb_cSize, NULL, RUBY_DEFAULT_FREE, size);
}

void rpg_font_measure_s(RPGfont *font, void *str, RPGsize *size) {
    memset(size, 0, sizeof(RPGsize));
    size_t len = utf8len(str);
    utf8_int32_t cp;
    RPGglyph *glyph;

    RPGfont_face *ff = NULL;
    HASH_FIND(face_handle, faces, &font->path, sizeof(ID), ff);
    if (ff == NULL) {
        rb_raise(rb_eRPGError, "disposed font");
    }

    RPGface_size *face_size = NULL;
    HASH_FIND(size_handle, ff->sizes, &font->size, sizeof(FT_UInt), face_size);
    if (face_size == NULL) {
        rb_raise(rb_eRPGError, "font not loaded");
    }

    for (size_t i = 0; i < len; i++) {
        str = utf8codepoint(str, &cp);
        glyph = rpg_font_glyph_inline(face_size, cp, ff->face);
        if (glyph) {
            if (glyph->size.height > size->height) {
                size->height = glyph->size.height;
            }
            size->width += glyph->advance >> 6;
        }
    }
}

