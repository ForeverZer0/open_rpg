#include "./font.h"

VALUE rb_cFont;
VALUE default_font;

FT_Library ft_lib;

GLuint _font_program;
GLint _font_projection;
GLint _font_color;
GLuint vao;
GLuint vbo;

#define VERTEX_SHADER "./shaders/font.vert"
#define FRAGMENT_SHADER "./shaders/font.frag"

#define VERTEX_LENGTH (24)
#define VERTEX_SIZE (sizeof(float) * VERTEX_LENGTH)

void rpg_font_init(VALUE parent) {
    rb_cFont = rb_define_class_under(parent, "Font", rb_cObject);
    rb_define_alloc_func(rb_cFont, rpg_font_alloc);

    rb_define_method(rb_cFont, "initialize", rpg_font_initialize, 2);
    rb_define_method(rb_cFont, "dispose", rpg_font_dispose, 0);
    rb_define_method(rb_cFont, "measure", rpg_font_measure, 1);

    rb_define_method(rb_cFont, "name", rpg_font_name, 0);
    rb_define_method(rb_cFont, "size", rpg_font_size, 0);
    rb_define_method(rb_cFont, "bold", rpg_font_bold, 0);
    rb_define_method(rb_cFont, "italic", rpg_font_italic, 0);
    rb_define_method(rb_cFont, "color", rpg_font_get_color, 0);
    rb_define_method(rb_cFont, "color=", rpg_font_set_color, 1);

    rb_define_singleton_method(rb_cFont, "default", rpg_font_get_default, 0);
    rb_define_singleton_method(rb_cFont, "default=", rpg_font_set_default, 1);

    rb_include_module(rb_cFont, rb_mDisposable);

    if (FT_Init_FreeType(&ft_lib)) {
        rb_raise(rb_eRPGError, "failed to load FreeType library");
    }

    default_font = Qnil;
    vao = 0;
    vbo = 0;
}

ALLOC_FUNC(rpg_font_alloc, RPGfont)

void rpg_font_terminate(void) {
    if (ft_lib) {
        FT_Done_FreeType(ft_lib);
    }
}

static inline RPGglyph *rpg_font_glyph_inline(RPGfont *font, int codepoint) {
    RPGglyph *glyph = NULL;
    HASH_FIND_INT(font->glyphs, &codepoint, glyph);
    if (glyph == NULL) {
        glyph = ALLOC(RPGglyph);
        glyph->codepoint = codepoint;

        FT_Load_Char(font->face, codepoint, FT_LOAD_RENDER);
        glyph->size.width = font->face->glyph->bitmap.width;
        glyph->size.height = font->face->glyph->bitmap.rows;

        glGenTextures(1, &glyph->texture);
        glBindTexture(GL_TEXTURE_2D, glyph->texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, glyph->size.width, glyph->size.height, 0, GL_RED, GL_UNSIGNED_BYTE,
                     font->face->glyph->bitmap.buffer);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);

        glyph->bearing.x = font->face->glyph->bitmap_left;
        glyph->bearing.y = font->face->glyph->bitmap_top;
        glyph->advance = (GLint)font->face->glyph->advance.x;

        HASH_ADD_INT(font->glyphs, codepoint, glyph);
    }
    return glyph;
}

static VALUE rpg_font_initialize(VALUE self, VALUE path, VALUE px_size) {

    VALUE absolute = rb_file_s_absolute_path(1, &path);
    char *fname = StringValueCStr(absolute);
    RPGfont *font = DATA_PTR(self);

    if (FT_New_Face(ft_lib, fname, 0, &font->face)) {
        rb_raise(rb_eRPGError, "failed to load font");
    }
    font->pixel_size = (FT_UInt)NUM2UINT(px_size);

    FT_Set_Pixel_Sizes(font->face, 0, font->pixel_size);
    FT_Load_Char(font->face, 'H', FT_LOAD_RENDER);
    font->v_offset = font->face->glyph->bitmap_top;

    if (!vao) {
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, VERTEX_SIZE, NULL, GL_DYNAMIC_DRAW);
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
    return Qnil;
}

static VALUE rpg_font_dispose(VALUE self) {
    RPGfont *font = DATA_PTR(self);
    if (font->glyphs) {
        RPGglyph *s, *tmp;
        HASH_ITER(hh, font->glyphs, s, tmp) {
            if (s->texture) {
                glDeleteTextures(1, &s->texture);
            }
            xfree(s);
        }
        xfree(font->glyphs);
        font->glyphs = NULL;
    }
    if (font->face) {
        FT_Done_Face(font->face);
    }
    if (font->color) {
        xfree(font->color);
    }
    return Qnil;
}

void rpg_font_measure_s(RPGfont *font, void *str, RPGsize *size) {
    memset(size, 0, sizeof(RPGsize));
    size_t len = utf8len(str);
    utf8_int32_t cp;
    RPGglyph *glyph;
    for (size_t i = 0; i < len; i++) {
        str = utf8codepoint(str, &cp);
        glyph = rpg_font_glyph_inline(font, cp);
        if (glyph) {
            if (glyph->size.height > size->height) {
                size->height = glyph->size.height;
            }
            size->width += glyph->advance >> 6;
        }
    }
}

static VALUE rpg_font_measure(VALUE self, VALUE text) {
    RPGfont *font = DATA_PTR(self);
    void *str = StringValueCStr(text);
    RPGsize *size = ALLOC(RPGsize);
    rpg_font_measure_s(font, str, size);
    return Data_Wrap_Struct(rb_cSize, NULL, RUBY_DEFAULT_FREE, size);
}

static VALUE rpg_font_size(VALUE self) {
    RPGfont *font = DATA_PTR(self);
    return UINT2NUM(font->pixel_size);
}

static VALUE rpg_font_name(VALUE self) {
    RPGfont *font = DATA_PTR(self);
    const char *name = font->face->family_name;
    return rb_str_new_cstr(name);
}

static VALUE rpg_font_bold(VALUE self) {
    RPGfont *font = DATA_PTR(self);
    return RB_BOOL((font->face->style_flags & FT_STYLE_FLAG_BOLD) != 0);
}

static VALUE rpg_font_italic(VALUE self) {
    RPGfont *font = DATA_PTR(self);
    return RB_BOOL((font->face->style_flags & FT_STYLE_FLAG_ITALIC) != 0);
}

void rpg_font_render(RPGfont *font, RPGmatrix4x4 *ortho, const char *text, int x, int y) {
    glUseProgram(_font_program);

    glUniformMatrix4fv(_font_projection, 1, GL_FALSE, (float *)ortho);

    if (font->color) {
        glUniform4f(_font_color, font->color->r, font->color->g, font->color->b, font->color->a);
    } else if (!NIL_P(default_font)) {
        RPGfont *df = DATA_PTR(default_font);
        glUniform4f(_font_color, df->color->r, df->color->g, df->color->b, df->color->a);
    } else {
        glUniform4f(_font_color, 0.0f, 0.0f, 0.0f, 0.0f);
    }

    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(vao);

    size_t len = utf8len(text);
    void *str = (void *)text;
    utf8_int32_t cp;
    RPGglyph *ch;
    float ox = (float)x;

    for (size_t i = 0; i < len; i++) {

        str = utf8codepoint(str, &cp);
        ch = rpg_font_glyph_inline(font, cp);

        float xPos = ox + ch->bearing.x;
        float yPos = y + (font->v_offset - ch->bearing.y);
        float w = ch->size.width;
        float h = ch->size.height;
        glBindTexture(GL_TEXTURE_2D, ch->texture);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        float vertices[VERTEX_LENGTH] = {xPos, yPos + h, 0.0f, 1.0f, xPos + w, yPos,     1.0f, 0.0f, xPos,     yPos, 0.0f, 0.0f,
                                         xPos, yPos + h, 0.0f, 1.0f, xPos + w, yPos + h, 1.0f, 1.0f, xPos + w, yPos, 1.0f, 0.0f};

        glBufferSubData(GL_ARRAY_BUFFER, 0, VERTEX_SIZE, vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        ox += ch->advance >> 6;
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

static VALUE rpg_font_get_color(VALUE self) {
    RPGfont *font = DATA_PTR(self);
    if (font->color) {
        RPGcolor *color = ALLOC(RPGcolor);
        memcpy(color, font->color, sizeof(RPGcolor));
        return Data_Wrap_Struct(rb_cColor, NULL, RUBY_DEFAULT_FREE, color);
    }
    return Qnil;
}

static VALUE rpg_font_set_color(VALUE self, VALUE value) {
    RPGfont *font = DATA_PTR(self);
    if (NIL_P(value)) {
        if (font->color) {
            xfree(font->color);
            font->color = NULL;
        }
    } else {
        if (font->color == NULL) {
            font->color = ALLOC(RPGcolor);
        }
        RPGcolor *color = DATA_PTR(value);
        memcpy(font->color, color, sizeof(RPGcolor));
    }
    return value;
}

VALUE rpg_font_get_default(VALUE klass) {
    if (NIL_P(default_font) && access(DEFAULT_FONT_PATH, F_OK) != -1) {
        default_font = rpg_font_alloc(rb_cFont);
        rpg_font_initialize(default_font, rb_str_new2(DEFAULT_FONT_PATH), UINT2NUM(DEFAULT_FONT_SIZE));
        RPGfont *font = DATA_PTR(default_font);
        font->color = ALLOC(RPGcolor);
        font->color->r = 1.0f;
        font->color->g = 1.0f;
        font->color->b = 1.0f;
        font->color->a = 1.0f;
    }
    return default_font;
}

static VALUE rpg_font_set_default(VALUE klass, VALUE value) {
    if (RB_IS_A(value, rb_cFont) || NIL_P(value)) {
        default_font = value;
    } else {
        rb_raise(rb_eTypeError, "font expected");
    }
}