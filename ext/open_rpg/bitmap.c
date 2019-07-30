#include "./bitmap.h"

VALUE rb_cBitmap;

#define JPEG_QUALITY 95

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#define STBI_MALLOC xmalloc
#define STBI_REALLOC xrealloc
#define STBI_FREE xfree
#define STBIW_MALLOC STBI_MALLOC
#define STBIW_REALLOC STBI_REALLOC
#define STBIW_FREE STBI_FREE

#include "./stb_image.h"
#include "./stb_image_write.h"

void rpg_bitmap_init(VALUE parent) {
    rb_cBitmap = rb_define_class_under(parent, "Bitmap", rb_cObject);
    rb_define_alloc_func(rb_cBitmap, rpg_bitmap_alloc);
    rb_define_method(rb_cBitmap, "initialize", rpg_bitmap_initialize, -1);
    rb_define_method(rb_cBitmap, "dispose", rpg_bitmap_dispose, 0);
    rb_define_method(rb_cBitmap, "disposed?", rpg_bitmap_disposed_p, 0);

    rb_define_method(rb_cBitmap, "width", rpg_bitmap_width, 0);
    rb_define_method(rb_cBitmap, "height", rpg_bitmap_height, 0);
    rb_define_method(rb_cBitmap, "size", rpg_bitmap_size, 0);
    rb_define_method(rb_cBitmap, "rect", rpg_bitmap_rect, 0);
    rb_define_method(rb_cBitmap, "columns", rpg_bitmap_width, 0);
    rb_define_method(rb_cBitmap, "rows", rpg_bitmap_height, 0);
    rb_define_method(rb_cBitmap, "to_blob", rpg_bitmap_blob, 0);

    rb_define_method(rb_cBitmap, "get_pixel", rpg_bitmap_get_pixel, -1);
    rb_define_method(rb_cBitmap, "set_pixel", rpg_bitmap_set_pixel, -1);
    rb_define_method(rb_cBitmap, "fill_rect", rpg_bitmap_fill_rect, -1);
    rb_define_method(rb_cBitmap, "blt", rpg_bitmap_blt, -1);
    rb_define_method(rb_cBitmap, "stretch_blt", rpg_bitmap_stretch_blt, -1);
    rb_define_method(rb_cBitmap, "slice", rpg_bitmap_slice, -1);
    rb_define_method(rb_cBitmap, "clear", rpg_bitmap_clear, 0);
    rb_define_method(rb_cBitmap, "save", rpg_bitmap_save, 2);

    rb_define_method(rb_cBitmap, "font", rpg_bitmap_get_font, 0);
    rb_define_method(rb_cBitmap, "font=", rpg_bitmap_set_font, 1);
    rb_define_method(rb_cBitmap, "draw_text", rpg_bitmap_draw_text, -1);

    rb_define_singleton_method(rb_cBitmap, "from_blob", rpg_bitmap_from_blob, -1);

    rb_define_const(rb_cBitmap, "FORMAT_PNG", INT2NUM(RPG_FORMAT_PNG));
    rb_define_const(rb_cBitmap, "FORMAT_JPG", INT2NUM(RPG_FORMAT_JPG));
    rb_define_const(rb_cBitmap, "FORMAT_BMP", INT2NUM(RPG_FORMAT_BMP));

    rb_define_const(rb_cBitmap, "TYPE_RGBA", INT2NUM(GL_RGBA));
    rb_define_const(rb_cBitmap, "TYPE_RGB", INT2NUM(GL_RGB));
    rb_define_const(rb_cBitmap, "TYPE_BGRA", INT2NUM(GL_BGRA));
    rb_define_const(rb_cBitmap, "TYPE_BGR", INT2NUM(GL_BGR));

    rb_include_module(rb_cBitmap, rb_mDisposable);
}

static inline GLuint rpg_bitmap_fbo(RPGbitmap *bmp) {
    if (!bmp->fbo) {
        if (!bmp->texture) {
            rb_raise(rb_eRPGError, "disposed bitmap");
        }
        glGenFramebuffers(1, &bmp->fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, bmp->fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bmp->texture, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    return bmp->fbo;
}

static inline GLuint rpg_bitmap_generate(int width, int height, void *pixels, int format) {
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format, GL_UNSIGNED_BYTE, pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    return texture;
}

static inline void rpg_bitmap_fill_inline(RPGbitmap *bmp, int x, int y, int width, int height, RPGcolor *color) {
    rpg_bitmap_bind_fbo(bmp, x, y, width, height);
    glClearColor(color->r, color->g, color->b, color->a);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    rpg_bitmap_unbind_fbo();
}

void rpg_bitmap_bind_fbo(RPGbitmap *bmp, int x, int y, int width, int height) {
    GLuint fbo = rpg_bitmap_fbo(bmp);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(x, y, width, height);
    glScissor(x, y, width, height);
}

void rpg_bitmap_unbind_fbo(void) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(bounds.x, bounds.y, bounds.width, bounds.height);
    glScissor(bounds.x, bounds.y, bounds.width, bounds.height);
}

void *rpg_bitmap_load(const char *fname, int *width, int *height) { return stbi_load(fname, width, height, NULL, 4); }

void rpg_bitmap_free(void *data) {
    if (data) {
        RPGbitmap *bmp = data;
        if (bmp->texture) {
            glDeleteTextures(1, &bmp->texture);
        }
        if (bmp->fbo) {
            glDeleteFramebuffers(1, &bmp->fbo);
        }
    }
    xfree(data);
}

static VALUE rpg_bitmap_alloc(VALUE klass) {
    RPGbitmap *bmp = ALLOC(RPGbitmap);
    memset(bmp, 0, sizeof(RPGbitmap));
    return Data_Wrap_Struct(klass, NULL, rpg_bitmap_free, bmp);
}

void *rpg_bitmap_pixels(RPGbitmap *bitmap, int *size) {
    *size = BYTES_PER_PIXEL * bitmap->width * bitmap->height;
    void *pixels = xmalloc(*size);
    rpg_bitmap_bind_fbo(bitmap, 0, 0, bitmap->width, bitmap->height);
    glReadPixels(0, 0, bitmap->width, bitmap->height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    rpg_bitmap_unbind_fbo();
    return pixels;
}

static VALUE rpg_bitmap_blob(VALUE self) {
    RPGbitmap *bitmap = DATA_PTR(self);
    int size;
    void *pixels = rpg_bitmap_pixels(bitmap, &size);
    return rb_str_new(pixels, (long)size);
}

static VALUE rpg_bitmap_save(VALUE self, VALUE path, VALUE format) {
    RPGbitmap *bitmap = DATA_PTR(self);
    int size;
    void *pixels = rpg_bitmap_pixels(bitmap, &size);
    const char *fname = StringValueCStr(path);
    int fmt = NUM2INT(format);
    int result;
    switch (fmt) {
        case RPG_FORMAT_PNG: {
            result = stbi_write_png(fname, bitmap->width, bitmap->height, 4, pixels, bitmap->width * BYTES_PER_PIXEL);
            break;
        }
        case RPG_FORMAT_BMP: {
            result = stbi_write_bmp(fname, bitmap->width, bitmap->height, 4, pixels);
            break;
        }
        case RPG_FORMAT_JPG: {
            result = stbi_write_jpg(fname, bitmap->width, bitmap->height, 4, pixels, JPEG_QUALITY);
            break;
        }
        default: {
            rb_raise(rb_eArgError, "invalid format specified");
            return Qnil;
        }
    }
    xfree(pixels);
    if (!result) {
        rb_raise(rb_eRPGError, "failed to save bitmap");
    }
    return self;
}

static VALUE rpg_bitmap_dispose(VALUE self) {
    RPGbitmap *bmp = DATA_PTR(self);
    if (bmp->texture) {
        glDeleteTextures(1, &bmp->texture);
        bmp->texture = 0;
    }
    if (bmp->fbo) {
        glDeleteFramebuffers(1, &bmp->fbo);
        bmp->fbo = 0;
    }
}

static VALUE rpg_bitmap_disposed_p(VALUE self) {
    RPGbitmap *bmp = DATA_PTR(self);
    return bmp->texture ? Qfalse : Qtrue;
}

static VALUE rpg_bitmap_width(VALUE self) {
    RPGbitmap *bmp = DATA_PTR(self);
    return INT2NUM(bmp->width);
}

static VALUE rpg_bitmap_height(VALUE self) {
    RPGbitmap *bmp = DATA_PTR(self);
    return INT2NUM(bmp->height);
}

static VALUE rpg_bitmap_size(VALUE self) {
    RPGbitmap *bmp = DATA_PTR(self);
    RPGsize *size = ALLOC(RPGsize);
    size->width = bmp->width;
    size->height = bmp->height;
    return Data_Wrap_Struct(rb_cSize, NULL, RUBY_DEFAULT_FREE, size);
}

static VALUE rpg_bitmap_rect(VALUE self) {
    RPGbitmap *bmp = DATA_PTR(self);
    RPGrect *rect = ALLOC(RPGrect);
    rect->x = 0;
    rect->y = 0;
    rect->width = bmp->width;
    rect->height = bmp->height;
    return Data_Wrap_Struct(rb_cRect, NULL, RUBY_DEFAULT_FREE, rect);
}

static VALUE rpg_bitmap_initialize(int argc, VALUE *argv, VALUE self) {

    VALUE arg1, arg2, arg3;
    rb_scan_args(argc, argv, "12", &arg1, &arg2, &arg3);

    RPGbitmap *bitmap = DATA_PTR(self);
    void *pixels;

    if (argc == 1) {
        Check_Type(arg1, T_STRING);
        const char *fname = StringValueCStr(arg1);
        if (access(fname, F_OK) == -1) {
            VALUE enoent = rb_const_get(rb_mErrno, rb_intern("ENOENT"));
            rb_raise(enoent, "Cannot find file: %s", fname);
        }
        pixels = stbi_load(fname, &bitmap->width, &bitmap->height, NULL, BYTES_PER_PIXEL);
    } else {
        bitmap->width = NUM2INT(arg1);
        bitmap->height = NUM2INT(arg2);
        check_dimensions(bitmap->width, bitmap->height);
        int size = bitmap->width * bitmap->height;
        pixels = xmalloc(size * BYTES_PER_PIXEL);

        if (RTEST(arg3)) {
            RPGcolor *color = DATA_PTR(arg3);
            GLuint *data = (GLuint *)pixels;

            GLubyte r = (GLubyte)(color->r * 255);
            GLubyte g = (GLubyte)(color->g * 255);
            GLubyte b = (GLubyte)(color->b * 255);
            GLubyte a = (GLubyte)(color->a * 255);
            GLuint value = (r << 0) | (g << 8) | (b << 16) | (a << 24);

            for (int i = 0; i < size; i++) {
                data[i] = value;
            }
        } else {
            memset(pixels, 0, size * BYTES_PER_PIXEL);
        }
    }

    bitmap->texture = rpg_bitmap_generate(bitmap->width, bitmap->height, pixels, GL_RGBA);
    xfree(pixels);
    return Qnil;
}

static VALUE rpg_bitmap_from_blob(int argc, VALUE *argv, VALUE klass) {
    VALUE columns, rows, blob, format;
    rb_scan_args(argc, argv, "31", &columns, &rows, &blob, &format);

    RPGbitmap *bitmap = ALLOC(RPGbitmap);
    bitmap->width = NUM2INT(columns);
    bitmap->height = NUM2INT(rows);
    check_dimensions(bitmap->width, bitmap->height);

    int fmt = (argc == 3) ? GL_RGBA : NUM2INT(format);
    void *pixels = StringValuePtr(blob);
    bitmap->texture = rpg_bitmap_generate(bitmap->width, bitmap->height, pixels, fmt);

    return Data_Wrap_Struct(klass, NULL, RUBY_DEFAULT_FREE, bitmap);
}

static VALUE rpg_bitmap_get_pixel(int argc, VALUE *argv, VALUE self) {
    VALUE a1, a2;
    rb_scan_args(argc, argv, "11", &a1, &a2);
    int x, y;
    if (argc == 2) {
        x = NUM2INT(a1);
        y = NUM2INT(a2);
    } else {
        RPGpoint *point = DATA_PTR(a1);
        x = point->x;
        y = point->y;
    }

    RPGbitmap *bmp = DATA_PTR(self);
    if (x < 0 || x >= bmp->width || y < 0 || y >= bmp->height) {
        rb_raise(rb_eArgError, "location is outside of bitmap");
    }

    GLuint c;
    rpg_bitmap_bind_fbo(bmp, 0, 0, bmp->width, bmp->height);
    glReadPixels(x, y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &c);
    rpg_bitmap_unbind_fbo();

    RPGcolor *color = ALLOC(RPGcolor);
    color->r = ((c >> 24) & 0xFF) / 255.0f;
    color->g = ((c >> 16) & 0xFF) / 255.0f;
    color->b = ((c >> 8) & 0xFF) / 255.0f;
    color->a = ((c >> 0) & 0xFF) / 255.0f;
    return Data_Wrap_Struct(rb_cColor, NULL, RUBY_DEFAULT_FREE, color);
}

static VALUE rpg_bitmap_set_pixel(int argc, VALUE *argv, VALUE self) {
    VALUE a1, a2, a3;
    rb_scan_args(argc, argv, "21", &a1, &a2, &a3);
    int x, y;
    if (argc == 2) {
        x = NUM2INT(a1);
        y = NUM2INT(a2);
    } else {
        RPGpoint *point = DATA_PTR(a1);
        x = point->x;
        y = point->y;
    }

    RPGbitmap *bmp = DATA_PTR(self);
    if (x < 0 || x >= bmp->width || y < 0 || y >= bmp->height) {
        rb_raise(rb_eArgError, "location is outside of bitmap");
    }

    RPGcolor *color = DATA_PTR(self);
    rpg_bitmap_fill_inline(bmp, x, y, 1, 1, color);
    return self;
}

static VALUE rpg_bitmap_fill_rect(int argc, VALUE *argv, VALUE self) {
    VALUE a1, a2, a3, a4, a5;
    rb_scan_args(argc, argv, "23", &a1, &a2, &a3, &a4, &a5);

    int x, y, width, height;
    RPGcolor *color;
    switch (argc) {
        case 2: {
            RPGrect *rect = DATA_PTR(a1);
            x = rect->x;
            y = rect->y;
            width = rect->width;
            height = rect->height;
            color = DATA_PTR(a2);
            break;
        }
        case 3: {
            RPGpoint *point = DATA_PTR(a1);
            RPGsize *size = DATA_PTR(a2);
            x = point->x;
            y = point->y;
            width = size->width;
            height = size->height;
            color = DATA_PTR(a3);
            break;
        }
        case 5: {
            x = NUM2INT(a1);
            y = NUM2INT(a2);
            width = NUM2INT(a3);
            height = NUM2INT(a4);
            color = DATA_PTR(a5);
            break;
        }
        default: {
            rb_raise(rb_eArgError, "wrong number of arguments (given %d, expected 2, 3, 5)");
            break;
        }
    }
    RPGbitmap *bitmap = DATA_PTR(self);
    rpg_bitmap_fill_inline(bitmap, x, y, width, height, color);
    return self;
}

static VALUE rpg_bitmap_clear(VALUE self) {
    RPGbitmap *bmp = DATA_PTR(self);
    RPGcolor *color = ALLOC(RPGcolor);
    memset(color, 0, sizeof(RPGcolor));
    rpg_bitmap_fill_inline(bmp, 0, 0, bmp->width, bmp->height, color);
    return self;
}

static inline float rpg_bitmap_value2alpha(VALUE value) {
    float alpha;
    if (FIXNUM_P(value)) {
        alpha = NUM2INT(value) / 255.0f;
    } else {
        alpha = NUM2FLT(value);
    }
    return clampf(alpha, 0.0f, 1.0f);
}

static VALUE rpg_bitmap_slice(int argc, VALUE *argv, VALUE self) {
    VALUE a1, a2, a3, a4;
    rb_scan_args(argc, argv, "13", &a1, &a2, &a3, &a4);

    int x, y, w, h;
    switch (argc) {
        case 1: {
            RPGrect *rect = DATA_PTR(a1);
            x = rect->x;
            y = rect->y;
            w = rect->width;
            h = rect->height;
            break;
        }
        case 2: {
            RPGpoint *point = DATA_PTR(a1);
            RPGsize *size = DATA_PTR(a2);
            x = point->x;
            y = point->y;
            w = size->width;
            h = size->height;
            break;
        }
        case 4: {
            x = NUM2INT(a1);
            y = NUM2INT(a2);
            w = NUM2INT(a3);
            h = NUM2INT(a4);
            break;
        }
        default: {
            rb_raise(rb_eArgError, "wrong number of arguments (given %d, expected 1, 2, 4)", argc);
            break;
        }
    }
    check_dimensions(w, h);
    RPGbitmap *src = DATA_PTR(self);
    if (x + w > src->width || y + h > src->height || x < 0 || y < 0) {
        rb_raise(rb_eArgError, "bounds outside of source bitmap");
    }

    RPGbitmap *dst = ALLOC(RPGbitmap);
    dst->width = w;
    dst->height = h;
    dst->fbo = 0;

    void *pixels = xmalloc(BYTES_PER_PIXEL * w * h);
    rpg_bitmap_bind_fbo(src, x, y, w, h);
    glReadPixels(x, y, w, h, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    rpg_bitmap_unbind_fbo();

    dst->texture = rpg_bitmap_generate(w, h, pixels, GL_RGBA);
    xfree(pixels);

    return Data_Wrap_Struct(CLASS_OF(self), NULL, RUBY_DEFAULT_FREE, dst);
}

static VALUE rpg_bitmap_get_font(VALUE self) {
    RPGbitmap *bitmap = DATA_PTR(self);
    if (bitmap->font) {
        return Data_Wrap_Struct(rb_cFont, NULL, NULL, bitmap->font);
    }
    return Qnil;
}

static VALUE rpg_bitmap_set_font(VALUE self, VALUE value) {
    RPGbitmap *bitmap = DATA_PTR(self);
    if (NIL_P(value)) {
        bitmap->font = NULL;
    } else {
        bitmap->font = DATA_PTR(value);
    }
    return value;
}

// draw_text x, y, w, h, text, align = nil
// draw_text rect, text, align = nil

static VALUE rpg_bitmap_draw_text(int argc, VALUE *argv, VALUE self) {

    VALUE a1, a2, a3, a4, a5, a6;
    rb_scan_args(argc, argv, "24", &a1, &a2, &a3, &a4, &a5, &a6);

    RPGbitmap *bitmap = DATA_PTR(self);
    RPGfont *font;
    if (bitmap->font) {
        font = bitmap->font;
    } else {
        VALUE df = rpg_font_get_default(rb_cFont);
        if (NIL_P(df)) {
            return self;
        }
        font = DATA_PTR(df);
    }

    int x, y, w, h, align;
    const char *str;
    VALUE sz;
    switch (argc) {
        case 2: {
            RPGrect *rect = DATA_PTR(a1);
            x = rect->x;
            y = rect->y;
            w = rect->width;
            h = rect->height;
            str = StringValueCStr(a2);
            align = RPG_ALIGN_DEFAULT;
            break;
        }
        case 3: {
            RPGrect *rect = DATA_PTR(a1);
            x = rect->x;
            y = rect->y;
            w = rect->width;
            h = rect->height;
            str = StringValueCStr(a2);
            align = NUM2INT(a3);
            break;
        }
        case 5: {
            x = NUM2INT(a1);
            y = NUM2INT(a2);
            w = NUM2INT(a3);
            h = NUM2INT(a4);
            str = StringValueCStr(a5);
            align = RPG_ALIGN_DEFAULT;
            break;
        }
        case 6: {
            x = NUM2INT(a1);
            y = NUM2INT(a2);
            w = NUM2INT(a3);
            h = NUM2INT(a4);
            str = StringValueCStr(a5);
            align = RPG_ALIGN_DEFAULT;
            align = NUM2INT(a6);
            break;
        }
        default: {
            rb_raise(rb_eArgError, "wrong number of arguments (given %d, expected 2, 3, 5, 6)", argc);
            break;
        }
    }

    RPGsize dim;
    rpg_font_measure_s(font, (void *)str, &dim);

    switch (align) {
        case RPG_ALIGN_TOP_LEFT: {
            break;
        }
        case RPG_ALIGN_TOP_RIGHT: {
            x = w - x - dim.width;
            break;
        }
        case RPG_ALIGN_TOP_CENTER: {
            x += (w - dim.width) / 2;
            break;
        }
        case RPG_ALIGN_BOTTOM_LEFT: {
            y = h - y - dim.height;
            break;
        }
        case RPG_ALIGN_BOTTOM_RIGHT: {
            x = w - x - dim.width;
            y = h - y - dim.height;
            break;
        }
        case RPG_ALIGN_BOTTOM_CENTER: {
            x += (w - dim.width) / 2;
            y = h - y - dim.height;
            break;
        }
        case RPG_ALIGN_CENTER_LEFT: {
            y += (h - dim.height) / 2;
            break;
        }
        case RPG_ALIGN_CENTER_RIGHT: {
            x = w - x - dim.width;
            y += (h - dim.height) / 2;
            break;
        }
        case RPG_ALIGN_CENTER: {
            x += (w - dim.width) / 2;
            y += (h - dim.height) / 2;
            break;
        }
    }
    rpg_bitmap_bind_fbo(bitmap, 0, 0, bitmap->width, bitmap->height);
    rpg_font_render(font, str, x, y, w, h);
    rpg_bitmap_unbind_fbo();
    return self;
}

static VALUE rpg_bitmap_stretch_blt(int argc, VALUE *argv, VALUE self) {

    return Qnil;
}

static VALUE rpg_bitmap_blt(int argc, VALUE *argv, VALUE self) {
    VALUE a1, a2, a3, a4, a5;
    rb_scan_args(argc, argv, "41", &a1, &a2, &a3, &a4, &a5);

    int x = NUM2INT(a1);
    int y = NUM2INT(a2);
    RPGbitmap *dst = DATA_PTR(self);

    VALUE temp = Qnil;
    RPGbitmap *src = DATA_PTR(a3);;
    RPGrect *src_rect = DATA_PTR(a4);
    if (src_rect->x != 0 || src_rect->y != 0 || src_rect->width != src->width || src_rect->height != src->height) {
        temp = rpg_bitmap_slice(1, &a4, a3);
        src = DATA_PTR(temp);
    }
    float alpha = NIL_P(a5) ? 1.0f : rpg_bitmap_value2alpha(a5);

    rpg_bitmap_bind_fbo(dst, x, y, src_rect->width, src_rect->height);
    RPGmatrix4x4 *model = ALLOC(RPGmatrix4x4);

    MAT4_SET(model, game_width, 0.0f, 0.0f, 0.0f, 0.0f, -game_height, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, game_height, 0.0f, 1.0f);
    glUseProgram(_program);
    glUniformMatrix4fv(_model, 1, GL_FALSE, (float*) model);
    glUniform4f(_color, 0.0f, 0.0f, 0.0f, 0.0f);
    glUniform4f(_tone, 0.0f, 0.0f, 0.0f, 0.0f);
    glUniform1f(_alpha, alpha);
    glUniform4f(_flash, 0.0f, 0.0f, 0.0f, 0.0f);
    glUniform1i(_depth, 0xFFFF);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, src->texture);
    glBindVertexArray(quad_vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    if (temp != Qnil) {
        rpg_bitmap_dispose(temp);
    }

    xfree(model);
    rpg_bitmap_unbind_fbo();
    return self;
}