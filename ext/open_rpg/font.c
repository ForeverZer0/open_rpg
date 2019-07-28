#include "font.h"

VALUE rb_cFont;

RPGcolor default_color;
GLuint default_size;

FT_Library ft_lib;
GLuint font_program;
GLint proj_loc;
GLint color_loc;
GLuint vertex_array;
GLuint vertex_buffer;

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
    rb_define_method(rb_cFont, "bold", rpg_font_name, 0);
    rb_define_method(rb_cFont, "italic", rpg_font_name, 0);
    rb_define_method(rb_cFont, "color", rpg_font_get_color, 0);
    rb_define_method(rb_cFont, "color=", rpg_font_set_color, 1);
    rb_define_method(rb_cFont, "draw", rpg_font_draw, -1);

    rb_define_singleton_method(rb_cFont, "default_color", rpg_font_get_default_color, 0);
    rb_define_singleton_method(rb_cFont, "default_color=", rpg_font_set_default_color, 1);
    rb_define_singleton_method(rb_cFont, "default_size", rpg_font_get_default_size, 0);
    rb_define_singleton_method(rb_cFont, "default_size=", rpg_font_set_default_size, 1);

    if (FT_Init_FreeType(&ft_lib)) {
        rb_raise(rb_eRPGError, "failed to load FreeType library");
    }

    default_color.r = 1.0f;
    default_color.g = 1.0f;
    default_color.b = 1.0f;
    default_color.a = 1.0f;
    default_size = 24;
    vertex_array = 0;
    vertex_buffer = 0;
}

ALLOC_FUNC(rpg_font_alloc, RPGfont)

void rpg_font_terminate(void) {
    if (ft_lib) {
        FT_Done_FreeType(ft_lib);
    }
}

static inline RPGglyph* rpg_font_glyph_inline(RPGfont *font, int codepoint) {
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
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, glyph->size.width, glyph->size.height, 0, GL_RED, GL_UNSIGNED_BYTE, font->face->glyph->bitmap.buffer);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glBindTexture(GL_TEXTURE_2D, 0);

        glyph->bearing.x = font->face->glyph->bitmap_left;
        glyph->bearing.y = font->face->glyph->bitmap_top;
        glyph->advance = (GLint) font->face->glyph->advance.x;

        HASH_ADD_INT(font->glyphs, codepoint, glyph);
    }
    return glyph;
}

static VALUE rpg_font_initialize(VALUE self, VALUE path, VALUE px_size) {

    char *fname = StringValueCStr(path);
    RPGfont *font = DATA_PTR(self);

    if (FT_New_Face(ft_lib, fname, 0, &font->face)) {
        rb_raise(rb_eRPGError, "failed to load font");
    }
    font->pixel_size = NUM2UINT(px_size);

    FT_Set_Pixel_Sizes(font->face, 0, font->pixel_size);
    FT_Load_Char(font->face, 'H', FT_LOAD_RENDER);
    font->v_offset = font->face->glyph->bitmap_top;

    if (!vertex_array) {
        glGenVertexArrays(1, &vertex_array);
        glGenBuffers(1, &vertex_buffer);
        glBindVertexArray(vertex_array);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        glBufferData(GL_ARRAY_BUFFER, VERTEX_SIZE, NULL, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), NULL);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    if (!font_program) {
        const char *frag_src = "#version 330 core\nin vec2 coords;\nout vec4 result;\nuniform sampler2D text;\nuniform vec4 color;\nvoid main()\n{\nvec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, coords).r);\nresult = vec4(color) * sampled;\n}";
        const char *vert_src = "#version 330 core\nlayout (location = 0) in vec4 vertex;\nout vec2 coords;\nuniform mat4 projection;\nvoid main()\n{\ngl_Position = projection * vec4(vertex.xy, 0.0, 1.0);\ncoords = vertex.zw;\n}";
        font_program = glCreateProgram();
        GLuint vert = glCreateShader(GL_VERTEX_SHADER);
        GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);

        int len = (int) strlen(vert_src);
        glShaderSource(vert, 1, &vert_src, &len);
        len = (int) strlen(frag_src);
        glShaderSource(frag, 1, &frag_src, &len);

        glCompileShader(vert);
        glCompileShader(frag);

        glAttachShader(font_program, vert);
        glAttachShader(font_program, frag);

        glLinkProgram(font_program);
        glDeleteShader(vert);
        glDeleteShader(frag);

        proj_loc = glGetUniformLocation(font_program, "projection");
        color_loc = glGetUniformLocation(font_program, "color");
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

static VALUE rpg_font_measure(VALUE self, VALUE text) {
    RPGfont *font = DATA_PTR(self);
    RPGsize *size = ALLOC(RPGsize);
    memset(size, 0, sizeof(RPGsize));
    void *str = StringValueCStr(text);
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
    return Data_Wrap_Struct(rb_cSize, NULL, RUBY_DEFAULT_FREE, size);
}

static VALUE rpg_font_size(VALUE self) {
    RPGfont *font = DATA_PTR(self);
    return UINT2NUM(font->pixel_size);
}

static VALUE rpg_font_name(VALUE self) {
    RPGfont *font = DATA_PTR(self);
    const char * name = font->face->family_name;
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

void rpg_font_render(RPGfont *font, const char *text, int x, int y, int width, int height) {
    glUseProgram(font_program);
    RPGmatrix4x4 ortho;
    rpg_mat4_create_ortho(&ortho, 0.0f, width, 0.0f, height, -1.0f, 1.0f);
    glUniformMatrix4fv(proj_loc, 1, GL_FALSE, (float*) &ortho);
    if (font->color) {
        glUniform4f(color_loc, font->color->r, font->color->g, font->color->b, font->color->a);
    }
    else {
        glUniform4f(color_loc, default_color.r, default_color.g, default_color.b, default_color.a);
    }

    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(vertex_array);

    size_t len = utf8len(text);
    void* str = (void*) text;
    utf8_int32_t cp;
    RPGglyph *ch;
    float ox = (float) x;

    for (size_t i = 0; i < len; i++) {

        str = utf8codepoint(str, &cp);
        ch = rpg_font_glyph_inline(font, cp);

        float xPos = ox + ch->bearing.x;
        float yPos = y + (font->v_offset - ch->bearing.y);
        float w = ch->size.width;
        float h = ch->size.height;
        glBindTexture(GL_TEXTURE_2D, ch->texture);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);

        float vertices[VERTEX_LENGTH] =
        {
            xPos, yPos + h, 0.0f, 1.0f,
            xPos + w, yPos, 1.0f, 0.0f,
            xPos, yPos, 0.0f, 0.0f,
            xPos, yPos + h, 0.0f, 1.0f,
            xPos + w, yPos + h, 1.0f, 1.0f,
            xPos + w, yPos, 1.0f, 0.0f
        };

        glBufferSubData(GL_ARRAY_BUFFER, 0, VERTEX_SIZE, vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        ox += ch->advance >> 6;
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

static VALUE rpg_font_draw(int argc, VALUE *argv, VALUE self) {
    VALUE a1, a2, a3, a4, a5, a6, a7;
    rb_scan_args(argc, argv, "34", &a1, &a2, &a3, &a4, &a5, &a6, &a7);
    RPGfont *font = DATA_PTR(self);
    RPGimage *image = DATA_PTR(a1);
    if (image == NULL) {
        rb_raise(rb_eArgError, "image cannot be nil");
    }
    const char *str = StringValueCStr(a2);
    int x, y, w, h, align;
    switch (argc) {
        case 3: {
            RPGrect *rect = DATA_PTR(a3); 
            x = rect->x;
            y = rect->y;
            w = rect->width;
            h = rect->height;
            align = RPG_ALIGN_DEFAULT;
            break;
        }
        case 4: {
            RPGrect *rect = DATA_PTR(a3); 
            x = rect->x;
            y = rect->y;
            w = rect->width;
            h = rect->height;
            align = NUM2INT(a4);
            break;
        }
        case 6: {
            x = NUM2INT(a3);
            y = NUM2INT(a4);
            w = NUM2INT(a5);
            h = NUM2INT(a6);
            align = RPG_ALIGN_DEFAULT;
            break;
        }
        case 7: {
            x = NUM2INT(a3);
            y = NUM2INT(a4);
            w = NUM2INT(a5);
            h = NUM2INT(a6);
            align = NUM2INT(a7);
            break;
        }
        default: {
            rb_raise(rb_eArgError, "wrong number of arguments (given %d, expected 3, 4, 6, 7)", argc);
            break;
        }
    }
    VALUE sz = rpg_font_measure(self, a2);
    RPGsize *dim = DATA_PTR(sz);
    switch (align) {
        case RPG_ALIGN_TOP_LEFT: {
            break;
        }
        case RPG_ALIGN_TOP_RIGHT: {
            x = w - x - dim->width;
            break;
        }
        case RPG_ALIGN_TOP_CENTER: {
            x += (w - dim->width) / 2;
            break;
        }
        case RPG_ALIGN_BOTTOM_LEFT: {
            y = h - y - dim->height;
            break;
        }
        case RPG_ALIGN_BOTTOM_RIGHT: {
            x = w - x - dim->width;
            y = h - y - dim->height;
            break;
        }
        case RPG_ALIGN_BOTTOM_CENTER: {
            x += (w - dim->width) / 2;
            y = h - y - dim->height;
            break;
        }
        case RPG_ALIGN_CENTER_LEFT: {
            y += (h - dim->height) / 2;
            break;
        }
        case RPG_ALIGN_CENTER_RIGHT: {
            x = w - x - dim->width;
            y += (h - dim->height) / 2;
            break;
        }
        case RPG_ALIGN_CENTER: {
            x += (w - dim->width) / 2;
            y += (h - dim->height) / 2;
            break;
        }
    }
    rpg_image_bind_fbo(image, 0, 0, image->width, image->height);
    rpg_font_render(font, str, x, y, w, h);
    rpg_image_unbind_fbo();
    return self;
}

static VALUE rpg_font_get_default_color(VALUE klass) {
    RPGcolor *color = ALLOC(RPGcolor);
    memcpy(color, &default_color, sizeof(RPGcolor));
    return Data_Wrap_Struct(rb_cColor, NULL, RUBY_DEFAULT_FREE, color);
}

static VALUE rpg_font_set_default_color(VALUE klass, VALUE value) {
    RPGfont *color = DATA_PTR(value);
    memcpy(&default_color, color, sizeof(RPGcolor));
    return value;
}

static VALUE rpg_font_get_default_size(VALUE klass) {
    return UINT2NUM(default_size);
}

static VALUE rpg_font_set_default_size(VALUE klass, VALUE value) {
    default_size = NUM2UINT(value);
    if (default_size == 0) {
        default_size = 1;
    }
    return value;
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
    }
    else {
        if (font->color == NULL) {
            font->color = ALLOC(RPGcolor);
        }
        RPGcolor *color = DATA_PTR(value);
        memcpy(font->color, color, sizeof(RPGcolor)); 
    }
    return value;
}