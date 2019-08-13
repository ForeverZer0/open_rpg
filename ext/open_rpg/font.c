#include "./font.h"

VALUE rb_cFont;
VALUE rb_cGlyph;

GLuint _font_program;
GLint _font_projection;
GLint _font_color;
GLuint font_vao;
GLuint font_vbo;

FT_Library ft_lib;
FT_Stroker ft_stroker; // FIXME:

RPGfont_face *faces;
RPGfont default_font;

#define DEFAULT_FONT "NotoSans-Black.ttf"
#define DEFAULT_SIZE 18
#define VERTEX_SHADER "font.vert"
#define FRAGMENT_SHADER "font.frag"

void rpg_font_init(VALUE parent) {
    if (FT_Init_FreeType(&ft_lib)) {
        rb_raise(rb_eRPGError, "failed to load FreeType library");
    }
    if (FT_Stroker_New(ft_lib, &ft_stroker)) {
        rb_raise(rb_eRPGError, "failed to initialize FreeType stroker");
    }

    faces = NULL;
    font_vao = 0;
    font_vbo = 0;
    rpg_font_create_default();

    rb_cFont = rb_define_class_under(parent, "Font", rb_cObject);
    rb_define_alloc_func(rb_cFont, rpg_font_alloc);

    rb_define_method(rb_cFont, "measure", rpg_font_measure, 1);
    rb_define_method(rb_cFont, "name", rpg_font_name, 0);

    rb_define_method(rb_cFont, "size", rpg_font_get_size, 0);
    rb_define_method(rb_cFont, "size=", rpg_font_set_size, 1);
    rb_define_method(rb_cFont, "color", rpg_font_get_color, 0);
    rb_define_method(rb_cFont, "color=", rpg_font_set_color, 1);
    rb_define_method(rb_cFont, "bold", rpg_font_bold, 0);
    rb_define_method(rb_cFont, "italic", rpg_font_italic, 0);
    rb_define_method(rb_cFont, "glyph", rpg_font_glyph, 1);
    rb_define_method(rb_cFont, "each_glyph", rpg_font_each_glyph, 1);

    rb_cGlyph = rb_define_class_under(rb_cFont, "Glyph", rb_cObject);
    rb_funcall(rb_cGlyph, rb_intern("private_class_method"), 1, STR2SYM("new"));

    rb_define_method(rb_cGlyph, "codepoint", rpg_glyph_codepoint, 0);
    rb_define_method(rb_cGlyph, "texture", rpg_glyph_texture, 0);
    rb_define_method(rb_cGlyph, "size", rpg_glyph_size, 0);
    rb_define_method(rb_cGlyph, "bearing", rpg_glyph_bearing, 0);
    rb_define_method(rb_cGlyph, "advance", rpg_glyph_advance, 0);
    rb_define_method(rb_cGlyph, "inspect", rpg_glyph_inspect, 0);

    rb_define_singleton_method(rb_cFont, "default", rpg_font_get_default, 0);
    rb_define_singleton_method(rb_cFont, "default=", rpg_font_set_default, 1);
    rb_define_singleton_method(rb_cFont, "from_file", rpg_font_from_file, -1);
    rb_define_singleton_method(rb_cFont, "finalize", rpg_font_finalize, 0);
}

ALLOC_FUNC(rpg_font_alloc, RPGfont)

static VALUE rpg_font_finalize(VALUE klass) {
    return Qnil;
    if (font_vao) {
        glDeleteVertexArrays(1, &font_vao);
    }
    if (font_vbo) {
        glDeleteBuffers(1, &font_vbo);
    }
    if (_font_program) {
        glDeleteProgram(_font_program);
    }
    if (faces) {
        RPGfont_face *face, *tmp_face;
        HASH_ITER(face_handle, faces, face, tmp_face) {

            RPGface_size *size, *tmp_size;
            HASH_ITER(size_handle, face->sizes, size, tmp_size) {

                RPGglyph *glyph, *tmp_glyph;
                HASH_ITER(glyph_handle, size->glyphs, glyph, tmp_glyph) {
                    if (glyph->texture) {
                        glDeleteTextures(1, &glyph->texture);
                    }
                    xfree(glyph);
                }
                xfree(size->glyphs);
                xfree(size);
            }
            FT_Done_Face(face->face);
            xfree(face->sizes);
            xfree(face);
        }
        xfree(faces->sizes);
        xfree(faces);
    }
    FT_Done_FreeType(ft_lib);
    return Qnil;
}

static inline FT_Face rpg_font_face(RPGfont *font) {
    RPGfont_face *ff = NULL;
    HASH_FIND(face_handle, faces, &font->path, sizeof(ID), ff);
    if (ff == NULL) {
        rb_raise(rb_eRPGError, "disposed font");
    }
    return ff->face;
}

static inline RPGfont_face *rpg_font_load_face(const char *fname, ID id) {
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
    return ff;
}

static inline RPGface_size *rpg_font_load_size(RPGfont_face *ff, FT_UInt size) {
    RPGface_size *face_size = NULL;
    HASH_FIND(size_handle, ff->sizes, &size, sizeof(FT_UInt), face_size);
    if (face_size == NULL) {
        face_size = ALLOC(RPGface_size);
        face_size->size = size;
        face_size->glyphs = NULL;

        // Set the offset, using a character that (typically) extends from baseline to peak height
        // FT_Set_Pixel_Sizes(ff->face, 0, size);
        FT_Set_Char_Size(ff->face, 0, size * 64, 72, 72); // FIXME:
        FT_Load_Char(ff->face, 'H', FT_LOAD_RENDER);
        face_size->offset = ff->face->glyph->bitmap_top;
        HASH_ADD(size_handle, ff->sizes, size, sizeof(FT_UInt), face_size);
    }
    return face_size;
}

static VALUE rpg_font_from_file(int argc, VALUE *argv, VALUE klass) {

    VALUE path, sz, options;
    rb_scan_args(argc, argv, "11:", &path, &sz, &options);

    RPGfont *f = ALLOC(RPGfont);
    char *fname = rpg_expand_path_s(path);
    ID id = rb_intern(fname);
    FT_UInt size = NIL_P(sz) ? default_font.size : NUM2UINT(sz);
    if (size == 0) {
        rb_raise(rb_eArgError, "size must be greater than 0");
    }

    RPGfont_face *ff = rpg_font_load_face(fname, id);
    RPGface_size *face_size = rpg_font_load_size(ff, size);

    f->path = id;
    f->size = size;

    if (RTEST(options)) {
        VALUE opt = rb_hash_aref(options, STR2SYM("color"));
        if (NIL_P(opt)) {
            memcpy(&f->color, &default_font.color, sizeof(RPGcolor));
        } else {
            RPGcolor *c = DATA_PTR(opt);
            memcpy(&f->color, c, sizeof(RPGcolor));
        }

    } else {
        memcpy(&f->color, &default_font.color, sizeof(RPGcolor));
    }
    return Data_Wrap_Struct(klass, NULL, RUBY_DEFAULT_FREE, f);
}

static void rpg_font_create_default(void) {

    char *path = xmalloc(256);
    sprintf(path, "%s/%s", RPG_FONTS, DEFAULT_FONT);

    ID id = rb_intern(path);
    RPGfont_face *ff = rpg_font_load_face(path, id);
    rpg_font_load_size(ff, DEFAULT_SIZE);
    default_font.path = id;
    default_font.color.r = 1.0f;
    default_font.color.g = 1.0f;
    default_font.color.b = 1.0f;
    default_font.color.a = 1.0f;
    default_font.size = DEFAULT_SIZE;
    xfree(path);
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
    rpg_font_load_size(ff, sz);

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

        ///////////////// FIXME: Added

        FT_Stroker_Set(ft_stroker, 2 * 64, FT_STROKER_LINECAP_ROUND, FT_STROKER_LINEJOIN_ROUND, 0);
        FT_UInt glyphIndex = f->glyph->glyph_index;
        FT_Load_Glyph(f, glyphIndex, FT_LOAD_DEFAULT);
        FT_Glyph temp;
        FT_Get_Glyph(f->glyph, &temp);

        FT_Glyph_StrokeBorder(&temp, ft_stroker, GL_FALSE, GL_FALSE);
        FT_Glyph_To_Bitmap(&temp, FT_RENDER_MODE_NORMAL, NULL, GL_FALSE);
        FT_BitmapGlyph bmpGlyph = *((FT_BitmapGlyph*) &temp);

        g->outline.size.width  = bmpGlyph->bitmap.width;
        g->outline.size.height = bmpGlyph->bitmap.rows;

        g->outline.bearing.x = bmpGlyph->left;
        g->outline.bearing.y = bmpGlyph->top;
        g->outline.advance = (GLint)temp->advance.x; 
        
        glGenTextures(1, &g->outline.texture);
        glBindTexture(GL_TEXTURE_2D, g->outline.texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, g->outline.size.width, g->outline.size.height, 0, GL_RED, GL_UNSIGNED_BYTE, bmpGlyph->bitmap.buffer);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);

        FT_Done_Glyph(temp);

        ///////////////////////////////

        FT_Load_Char(f, codepoint, FT_LOAD_RENDER);

        g->size.width = f->glyph->bitmap.width;
        g->size.height = f->glyph->bitmap.rows;

        glGenTextures(1, &g->texture);
        glBindTexture(GL_TEXTURE_2D, g->texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, g->size.width, g->size.height, 0, GL_RED, GL_UNSIGNED_BYTE, f->glyph->bitmap.buffer);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
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
    if (font == NULL || font->path == 0) {
        rb_warn("font is NULL");
        // TODO: Remove
        return;
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

        char *vert_path = xmalloc(256);
        char *frag_path = xmalloc(256);
        sprintf(vert_path, "%s/%s", RPG_SHADERS, VERTEX_SHADER);
        sprintf(frag_path, "%s/%s", RPG_SHADERS, FRAGMENT_SHADER);

        _font_program = rpg_create_shader_program(vert_path, frag_path, NULL);
        _font_projection = glGetUniformLocation(_font_program, "projection");
        _font_color = glGetUniformLocation(_font_program, "color");

        xfree(vert_path);
        xfree(frag_path);
    }

    glUseProgram(_font_program);
    glUniformMatrix4fv(_font_projection, 1, GL_FALSE, (float *)ortho);
    glUniform4f(_font_color, font->color.r, font->color.g, font->color.b, font->color.a);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // TODO: Experiment...

    RPGfont_face *ff = NULL;
    HASH_FIND(face_handle, faces, &font->path, sizeof(ID), ff);
    if (ff == NULL) {
        rb_raise(rb_eRPGError, "disposed font");
    }

    RPGface_size *face_size = NULL;
    HASH_FIND(size_handle, ff->sizes, &font->size, sizeof(FT_UInt), face_size);
    FT_Set_Char_Size(ff->face, 0, font->size * 64, 72, 72);
    if (face_size == NULL) {
        rb_raise(rb_eRPGError, "font not loaded");
    }

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
    
        /////////////////////////
        glUniform4f(_font_color, 0.0f, 0.0f, 0.0f, 1.0f);
        float xOutPos = xPos + ch->outline.bearing.x;
        float yOutPos = yPos + (ch->bearing.y - ch->outline.bearing.y);
        float outW = ch->outline.size.width;
        float outH = ch->outline.size.height;

        glBindTexture(GL_TEXTURE_2D, ch->outline.texture);
        glBindBuffer(GL_ARRAY_BUFFER, font_vbo);
        float outline[VERTICES_COUNT] = 
        {
            xOutPos, yOutPos + outH, 0.0f, 1.0f, 
            xOutPos + outW, yOutPos,     1.0f, 0.0f, 
            xOutPos,     yOutPos, 0.0f, 0.0f,
            xOutPos, yOutPos + outH, 0.0f, 1.0f, 
            xOutPos + outW, yOutPos + outH, 1.0f, 1.0f, 
            xOutPos + outW, yOutPos, 1.0f, 0.0f
        };
        glBufferSubData(GL_ARRAY_BUFFER, 0, VERTICES_SIZE, outline);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        /////////////////////////////

        glUniform4f(_font_color, 1.0f, 1.0f, 1.0f, 1.0f);
        glBindTexture(GL_TEXTURE_2D, ch->texture);
        glBindBuffer(GL_ARRAY_BUFFER, font_vbo);
        float vertices[VERTICES_COUNT] = {xPos, yPos + h, 0.0f, 1.0f, xPos + w, yPos,     1.0f, 0.0f, xPos,     yPos, 0.0f, 0.0f,
                                          xPos, yPos + h, 0.0f, 1.0f, xPos + w, yPos + h, 1.0f, 1.0f, xPos + w, yPos, 1.0f, 0.0f};
        glBufferSubData(GL_ARRAY_BUFFER, 0, VERTICES_SIZE, vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        ox += ch->advance >> 6;
    }





    ///////////////////////////////////////////////////////////FIXME:
    // glUniform4f(_font_color, 0.0f, 0.0f, 0.0f, 1.0f);

    // for (size_t i = 0; i < len; i++) {

    //     str = utf8codepoint(str, &cp);
    //     ch = rpg_font_glyph_inline(face_size, cp, ff->face);

    //     float xPos = ox + ch->outline.bearing.x - 1;
    //     float yPos = y + (face_size->offset - ch->outline.bearing.y) - 1;
    //     float w = ch->outline.size.width;
    //     float h = ch->outline.size.height;
    //     glBindTexture(GL_TEXTURE_2D, ch->outline.texture);
    //     glBindBuffer(GL_ARRAY_BUFFER, font_vbo);

    //     float vertices[VERTICES_COUNT] = {xPos, yPos + h, 0.0f, 1.0f, xPos + w, yPos,     1.0f, 0.0f, xPos,     yPos, 0.0f, 0.0f,
    //                                       xPos, yPos + h, 0.0f, 1.0f, xPos + w, yPos + h, 1.0f, 1.0f, xPos + w, yPos, 1.0f, 0.0f};

    //     glBufferSubData(GL_ARRAY_BUFFER, 0, VERTICES_SIZE, vertices);
    //     glBindBuffer(GL_ARRAY_BUFFER, 0);
    //     glDrawArrays(GL_TRIANGLES, 0, 6);
    //     ox += ch->outline.advance >> 6;
    // }
    // glUniform4f(_font_color, 1.0f, 1.0f, 1.0f, 1.0f);
    // ///////////////////////////////////////////////////////////FIXME:

    // ox = (float)x;
    // str = (void *)text;

    // for (size_t i = 0; i < len; i++) {

    //     str = utf8codepoint(str, &cp);
    //     ch = rpg_font_glyph_inline(face_size, cp, ff->face);

    //     float xPos = ox + ch->bearing.x;
    //     float yPos = y + (face_size->offset - ch->bearing.y);
    //     float w = ch->size.width;
    //     float h = ch->size.height;
    //     glBindTexture(GL_TEXTURE_2D, ch->texture);
    //     glBindBuffer(GL_ARRAY_BUFFER, font_vbo);

    //     float vertices[VERTICES_COUNT] = {xPos, yPos + h, 0.0f, 1.0f, xPos + w, yPos,     1.0f, 0.0f, xPos,     yPos, 0.0f, 0.0f,
    //                                       xPos, yPos + h, 0.0f, 1.0f, xPos + w, yPos + h, 1.0f, 1.0f, xPos + w, yPos, 1.0f, 0.0f};

    //     glBufferSubData(GL_ARRAY_BUFFER, 0, VERTICES_SIZE, vertices);
    //     glBindBuffer(GL_ARRAY_BUFFER, 0);
    //     glDrawArrays(GL_TRIANGLES, 0, 6);
    //     ox += ch->advance >> 6;
    // }

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

static VALUE rpg_font_each_glyph(VALUE self, VALUE value) {
    RPGfont *font = DATA_PTR(self);
    char *str = StringValueCStr(value);
    size_t len = utf8len(str);
    utf8_int32_t cp;

    RETURN_ENUMERATOR(self, 1, &value);

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

    RPGglyph *glyph;
    for (size_t i = 0; i < len; i++) {
        str = utf8codepoint(str, &cp);
        glyph = rpg_font_glyph_inline(face_size, cp, ff->face);
        if (glyph) {
            RPGglyph *result = ALLOC(RPGglyph);
            memcpy(result, glyph, sizeof(RPGglyph));
            rb_yield(Data_Wrap_Struct(rb_cGlyph, NULL, RUBY_DEFAULT_FREE, result));
        } else {
            rb_yield(Qnil);
        }
    }
    return self;
}

static VALUE rpg_font_glyph(VALUE self, VALUE codepoint) {
    RPGfont *font = DATA_PTR(self);
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

    RPGglyph *glyph, *result = ALLOC(RPGglyph);
    glyph = rpg_font_glyph_inline(face_size, NUM2INT(codepoint), ff->face);
    if (glyph == NULL) {
        return Qnil;
    }
    memcpy(result, glyph, sizeof(RPGglyph));
    return Data_Wrap_Struct(rb_cGlyph, NULL, RUBY_DEFAULT_FREE, result);
}

static VALUE rpg_glyph_codepoint(VALUE self) {
    RPGglyph *glyph = DATA_PTR(self);
    return INT2NUM(glyph->codepoint);
}

static VALUE rpg_glyph_texture(VALUE self) {
    RPGglyph *glyph = DATA_PTR(self);
    return UINT2NUM(glyph->texture);
}

static VALUE rpg_glyph_size(VALUE self) {
    RPGglyph *glyph = DATA_PTR(self);
    RPGsize *size = ALLOC(RPGsize);
    memcpy(size, &glyph->size, sizeof(RPGsize));
    return Data_Wrap_Struct(rb_cSize, NULL, RUBY_DEFAULT_FREE, size);
}

static VALUE rpg_glyph_bearing(VALUE self) {
    RPGglyph *glyph = DATA_PTR(self);
    RPGpoint *point = ALLOC(RPGpoint);
    memcpy(point, &glyph->bearing, sizeof(RPGpoint));
    return Data_Wrap_Struct(rb_cPoint, NULL, RUBY_DEFAULT_FREE, point);
}

static VALUE rpg_glyph_advance(VALUE self) {
    RPGglyph *glyph = DATA_PTR(self);
    return INT2NUM(glyph->advance);
}

static VALUE rpg_glyph_inspect(VALUE self) {
    RPGglyph *g = DATA_PTR(self);
    return rb_sprintf("<Glyph: codepoint:%d size:%d,%d bearing:%d,%d advance:%d>", g->codepoint, g->size.width, g->size.height,
                      g->bearing.x, g->bearing.y, g->advance >> 6);
}

static VALUE rpg_font_get_default(VALUE klass) {
    return default_font.path == 0 ? Qnil : Data_Wrap_Struct(rb_cFont, NULL, NULL, &default_font);
}

static VALUE rpg_font_set_default(VALUE klass, VALUE value) {
    if (NIL_P(value)) {
        default_font.path = 0;
    } else {
        RPGfont *font = DATA_PTR(value);
        memcpy(&default_font, font, sizeof(RPGfont));
    }
}