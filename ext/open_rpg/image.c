#include "./image.h"

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

#define BIND_BITMAP(_bmp, _x, _y, _w, _h)                                                                                                  \
    glBindFramebuffer(GL_FRAMEBUFFER, fetch_fbo(_bmp));                                                                                    \
    RPGmatrix4x4 _m;                                                                                                                       \
    MAT4_ORTHO(_m, 0.0f, _w, 0.0f, _h, -1.0f, 1.0f);                                                                                       \
    glUseProgram(_program);                                                                                                                \
    glUniformMatrix4fv(_projection, 1, GL_FALSE, (float *)&_m);                                                                            \
    glViewport(_x, _y, _w, _h);                                                                                                            \
    glScissor(_x, _y, _w, _h)

#define UNBIND_BITMAP()                                                                                                                    \
    glBindFramebuffer(GL_FRAMEBUFFER, 0);                                                                                                  \
    glUseProgram(_program);                                                                                                                \
    glUniformMatrix4fv(_projection, 1, GL_FALSE, (float *)&projection);                                                                    \
    glViewport(bounds.x, bounds.y, bounds.width, bounds.height);                                                                           \
    glScissor(bounds.x, bounds.y, bounds.width, bounds.height)

void rpg_image_init(VALUE parent) {
    rb_cBitmap = rb_define_class_under(parent, "Image", rb_cObject);
    rb_define_alloc_func(rb_cBitmap, rpg_image_alloc);
    rb_define_method(rb_cBitmap, "initialize", rpg_image_initialize, -1);

    rb_define_method(rb_cBitmap, "width", rpg_image_width, 0);
    rb_define_method(rb_cBitmap, "height", rpg_image_height, 0);
    rb_define_method(rb_cBitmap, "size", rpg_image_size, 0);
    rb_define_method(rb_cBitmap, "rect", rpg_image_rect, 0);
    rb_define_method(rb_cBitmap, "columns", rpg_image_width, 0);
    rb_define_method(rb_cBitmap, "rows", rpg_image_height, 0);
    rb_define_method(rb_cBitmap, "to_blob", rpg_image_blob, 0);

    rb_define_method(rb_cBitmap, "get_pixel", rpg_image_get_pixel, -1);
    rb_define_method(rb_cBitmap, "set_pixel", rpg_image_set_pixel, -1);
    rb_define_method(rb_cBitmap, "fill_rect", rpg_image_fill_rect, -1);
    rb_define_method(rb_cBitmap, "blit", rpg_image_blit, -1);

    rb_define_method(rb_cBitmap, "slice", rpg_image_slice, -1);
    rb_define_method(rb_cBitmap, "clear", rpg_image_clear, 0);
    rb_define_method(rb_cBitmap, "save", rpg_image_save, 2);

    rb_define_method(rb_cBitmap, "font", rpg_image_get_font, 0);
    rb_define_method(rb_cBitmap, "font=", rpg_image_set_font, 1);
    rb_define_method(rb_cBitmap, "draw_text", rpg_image_draw_text, -1);

    rb_define_method(rb_cBitmap, "fbo", rpg_image_fbo, 0);
    rb_define_method(rb_cBitmap, "texture", rpg_image_texture, 0);

    rb_define_singleton_method(rb_cBitmap, "from_blob", rpg_image_from_blob, -1);
    rb_define_singleton_method(rb_cBitmap, "load", rpg_image_load, 1);

    rb_define_const(rb_cBitmap, "FORMAT_PNG", INT2NUM(RPG_FORMAT_PNG));
    rb_define_const(rb_cBitmap, "FORMAT_JPG", INT2NUM(RPG_FORMAT_JPG));
    rb_define_const(rb_cBitmap, "FORMAT_BMP", INT2NUM(RPG_FORMAT_BMP));

    rb_define_const(rb_cBitmap, "TYPE_RGBA", INT2NUM(GL_RGBA));
    rb_define_const(rb_cBitmap, "TYPE_RGB", INT2NUM(GL_RGB));
    rb_define_const(rb_cBitmap, "TYPE_BGRA", INT2NUM(GL_BGRA));
    rb_define_const(rb_cBitmap, "TYPE_BGR", INT2NUM(GL_BGR));

    VALUE align = rb_define_module_under(parent, "Align");
    rb_define_const(align, "NONE", INT2NUM(RPG_ALIGN_NONE));
    rb_define_const(align, "TOP", INT2NUM(RPG_ALIGN_TOP));
    rb_define_const(align, "CENTER_V", INT2NUM(RPG_ALIGN_CENTER_V));
    rb_define_const(align, "BOTTOM", INT2NUM(RPG_ALIGN_BOTTOM));
    rb_define_const(align, "LEFT", INT2NUM(RPG_ALIGN_LEFT));
    rb_define_const(align, "CENTER_H", INT2NUM(RPG_ALIGN_CENTER_H));
    rb_define_const(align, "RIGHT", INT2NUM(RPG_ALIGN_RIGHT));
    rb_define_const(align, "TOP_LEFT", INT2NUM(RPG_ALIGN_TOP_LEFT));
    rb_define_const(align, "TOP_RIGHT", INT2NUM(RPG_ALIGN_TOP_RIGHT));
    rb_define_const(align, "TOP_CENTER", INT2NUM(RPG_ALIGN_TOP_CENTER));
    rb_define_const(align, "BOTTOM_LEFT", INT2NUM(RPG_ALIGN_BOTTOM_LEFT));
    rb_define_const(align, "BOTTOM_RIGHT", INT2NUM(RPG_ALIGN_BOTTOM_RIGHT));
    rb_define_const(align, "BOTTOM_CENTER", INT2NUM(RPG_ALIGN_BOTTOM_CENTER));
    rb_define_const(align, "CENTER_LEFT", INT2NUM(RPG_ALIGN_CENTER_LEFT));
    rb_define_const(align, "CENTER_RIGHT", INT2NUM(RPG_ALIGN_CENTER_RIGHT));
    rb_define_const(align, "CENTER", INT2NUM(RPG_ALIGN_CENTER));
}

ATTR_READER(rpg_image_fbo, RPGimage, fbo, UINT2NUM)
ATTR_READER(rpg_image_texture, RPGimage, texture, UINT2NUM)

static inline GLuint fetch_fbo(RPGimage *img) {
    if (!img->fbo) {
        if (!img->texture) {
            rb_raise(rb_eRPGError, "disposed image");
        }
        glGenFramebuffers(1, &img->fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, img->fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, img->texture, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    return img->fbo;
}

static inline GLuint rpg_image_generate(int width, int height, void *pixels, int format) {
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

static inline void rpg_image_fill_inline(RPGimage *img, int x, int y, int width, int height, RPGcolor *color) {
    BIND_BITMAP(img, x, y, width, height);
    glClearColor(color->r, color->g, color->b, color->a);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    UNBIND_BITMAP();
}

static VALUE rpg_image_alloc(VALUE klass) {
    RPGimage *img = ALLOC(RPGimage);
    memset(img, 0, sizeof(RPGimage));
    return Data_Wrap_Struct(klass, NULL, rpg_image_free, img);
}

void *rpg_image_pixels(RPGimage *image, int *size) {
    *size = BYTES_PER_PIXEL * image->width * image->height;
    void *pixels = xmalloc(*size);
    BIND_BITMAP(image, 0, 0, image->width, image->height);
    glReadPixels(0, 0, image->width, image->height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    UNBIND_BITMAP();
    return pixels;
}

static VALUE rpg_image_blob(VALUE self) {
    RPGimage *image = DATA_PTR(self);
    int size;
    void *pixels = rpg_image_pixels(image, &size);
    return rb_str_new(pixels, (long)size);
}

static VALUE rpg_image_save(VALUE self, VALUE path, VALUE format) {
    RPGimage *image = DATA_PTR(self);
    int size;
    void *pixels = rpg_image_pixels(image, &size);
    const char *fname = StringValueCStr(path);
    int fmt = NUM2INT(format);
    int result;
    switch (fmt) {
        case RPG_FORMAT_PNG: {
            result = stbi_write_png(fname, image->width, image->height, 4, pixels, image->width * BYTES_PER_PIXEL);
            break;
        }
        case RPG_FORMAT_BMP: {
            result = stbi_write_bmp(fname, image->width, image->height, 4, pixels);
            break;
        }
        case RPG_FORMAT_JPG: {
            result = stbi_write_jpg(fname, image->width, image->height, 4, pixels, JPEG_QUALITY);
            break;
        }
        default: {
            rb_raise(rb_eArgError, "invalid format specified");
            return Qnil;
        }
    }
    xfree(pixels);
    if (!result) {
        rb_raise(rb_eRPGError, "failed to save image");
    }
    return Qnil;
}

static VALUE rpg_image_width(VALUE self) {
    RPGimage *img = DATA_PTR(self);
    return INT2NUM(img->width);
}

static VALUE rpg_image_height(VALUE self) {
    RPGimage *img = DATA_PTR(self);
    return INT2NUM(img->height);
}

static VALUE rpg_image_size(VALUE self) {
    RPGimage *img = DATA_PTR(self);
    RPGsize *size = ALLOC(RPGsize);
    size->width = img->width;
    size->height = img->height;
    return Data_Wrap_Struct(rb_cSize, NULL, RUBY_DEFAULT_FREE, size);
}

static VALUE rpg_image_rect(VALUE self) {
    RPGimage *img = DATA_PTR(self);
    RPGrect *rect = ALLOC(RPGrect);
    rect->x = 0;
    rect->y = 0;
    rect->width = img->width;
    rect->height = img->height;
    return Data_Wrap_Struct(rb_cRect, NULL, RUBY_DEFAULT_FREE, rect);
}

static VALUE rpg_image_load(VALUE klass, VALUE path) {
    const char *fname = StringValueCStr(path);
    RPGimage *img = ALLOC(RPGimage);
    if (access(fname, F_OK) == -1) {
        rb_raise(ENOENT, "\"%s\"", fname);
    }

    void *pixels = stbi_load(fname, &img->width, &img->height, NULL, 4);
    if (pixels == NULL) {
        rb_raise(rb_eRPGError, "failed to load image - %s", fname);
    }
    img->font = NULL;
    img->fbo = 0;
    img->texture = rpg_image_generate(img->width, img->height, pixels, GL_RGBA);
    xfree(pixels);

    return Data_Wrap_Struct(klass, NULL, rpg_image_free, img);
}

static VALUE rpg_image_initialize(int argc, VALUE *argv, VALUE self) {

    VALUE a1, a2, a3;
    rb_scan_args(argc, argv, "21", &a1, &a2, &a3);
    RPGimage *img = DATA_PTR(self);

    img->width = NUM2INT(a1);
    img->height = NUM2INT(a2);
    check_dimensions(img->width, img->height);

    size_t size = img->width * img->height;
    void *pixels = xmalloc(size * BYTES_PER_PIXEL);

    if (RTEST(a3)) {
        RPGcolor *color = DATA_PTR(a3);
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
    img->texture = rpg_image_generate(img->width, img->height, pixels, GL_RGBA);
    xfree(pixels);
    return Qnil;
}

static VALUE rpg_image_from_blob(int argc, VALUE *argv, VALUE klass) {
    VALUE columns, rows, blob, format;
    rb_scan_args(argc, argv, "31", &columns, &rows, &blob, &format);

    RPGimage *img = ALLOC(RPGimage);
    img->width = NUM2INT(columns);
    img->height = NUM2INT(rows);
    check_dimensions(img->width, img->height);

    int fmt = (argc == 3) ? GL_RGBA : NUM2INT(format);
    void *pixels = StringValuePtr(blob);
    img->texture = rpg_image_generate(img->width, img->height, pixels, fmt);

    return Data_Wrap_Struct(klass, NULL, rpg_image_free, img);
}

static VALUE rpg_image_get_pixel(int argc, VALUE *argv, VALUE self) {
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

    RPGimage *img = DATA_PTR(self);
    if (x < 0 || x >= img->width || y < 0 || y >= img->height) {
        rb_raise(rb_eArgError, "location is outside of image");
    }

    GLuint c;
    BIND_BITMAP(img, 0, 0, img->width, img->height);
    glReadPixels(x, y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &c);
    UNBIND_BITMAP();

    RPGcolor *color = ALLOC(RPGcolor);
    color->r = ((c >> 24) & 0xFF) / 255.0f;
    color->g = ((c >> 16) & 0xFF) / 255.0f;
    color->b = ((c >> 8) & 0xFF) / 255.0f;
    color->a = ((c >> 0) & 0xFF) / 255.0f;
    return Data_Wrap_Struct(rb_cColor, NULL, RUBY_DEFAULT_FREE, color);
}

static VALUE rpg_image_set_pixel(int argc, VALUE *argv, VALUE self) {
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

    RPGimage *img = DATA_PTR(self);
    if (x < 0 || x >= img->width || y < 0 || y >= img->height) {
        rb_raise(rb_eArgError, "location is outside of image");
    }

    RPGcolor *color = DATA_PTR(self);
    rpg_image_fill_inline(img, x, y, 1, 1, color);
    return self;
}

static VALUE rpg_image_fill_rect(int argc, VALUE *argv, VALUE self) {
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
    RPGimage *image = DATA_PTR(self);
    rpg_image_fill_inline(image, x, y, width, height, color);
    return self;
}

static VALUE rpg_image_clear(VALUE self) {
    RPGimage *img = DATA_PTR(self);
    RPGcolor *color = ALLOC(RPGcolor);
    memset(color, 0, sizeof(RPGcolor));
    rpg_image_fill_inline(img, 0, 0, img->width, img->height, color);
    return self;
}

static inline float rpg_image_value2alpha(VALUE value) {
    float alpha;
    if (FIXNUM_P(value)) {
        alpha = NUM2INT(value) / 255.0f;
    } else {
        alpha = NUM2FLT(value);
    }
    return clampf(alpha, 0.0f, 1.0f);
}

static VALUE rpg_image_slice(int argc, VALUE *argv, VALUE self) {
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
    RPGimage *src = DATA_PTR(self);
    if (x + w > src->width || y + h > src->height || x < 0 || y < 0) {
        rb_raise(rb_eArgError, "bounds outside of source image");
    }

    RPGimage *dst = ALLOC(RPGimage);
    dst->width = w;
    dst->height = h;
    dst->fbo = 0;

    void *pixels = xmalloc(BYTES_PER_PIXEL * w * h);
    BIND_BITMAP(src, x, y, w, h);
    glReadPixels(x, y, w, h, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    UNBIND_BITMAP();

    dst->texture = rpg_image_generate(w, h, pixels, GL_RGBA);
    xfree(pixels);

    return Data_Wrap_Struct(CLASS_OF(self), NULL, rpg_image_free, dst);
}

static VALUE rpg_image_get_font(VALUE self) {
    RPGimage *image = DATA_PTR(self);
    if (image->font) {
        return Data_Wrap_Struct(rb_cFont, NULL, rpg_font_free, image->font);
    }
    return Qnil;
}

static VALUE rpg_image_set_font(VALUE self, VALUE value) {
    RPGimage *image = DATA_PTR(self);
    if (NIL_P(value)) {
        image->font = NULL;
    } else {
        image->font = DATA_PTR(value);
    }
    return value;
}

// draw_text x, y, w, h, text, align = nil
// draw_text rect, text, align = nil

static VALUE rpg_image_draw_text(int argc, VALUE *argv, VALUE self) {

    VALUE a1, a2, a3, a4, a5, a6;
    rb_scan_args(argc, argv, "24", &a1, &a2, &a3, &a4, &a5, &a6);

    RPGimage *img = DATA_PTR(self);
    RPGfont *font;
    if (img->font) {
        font = img->font;
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
        case RPG_ALIGN_CENTER_H:
        case RPG_ALIGN_TOP:
        case RPG_ALIGN_TOP_CENTER: {
            x += (w - dim.width) / 2;
            break;
        }
        case RPG_ALIGN_BOTTOM_LEFT: {
            y = h - y - dim.height - (font->v_offset / 3);
            break;
        }
        case RPG_ALIGN_BOTTOM_RIGHT: {
            x = w - x - dim.width;
            y = h - y - dim.height - (font->v_offset / 3);
            break;
        }
        case RPG_ALIGN_BOTTOM:
        case RPG_ALIGN_BOTTOM_CENTER: {
            x += (w - dim.width) / 2;
            y = h - y - dim.height - (font->v_offset / 3);
            break;
        }
        case RPG_ALIGN_CENTER_V:
        case RPG_ALIGN_LEFT:
        case RPG_ALIGN_CENTER_LEFT: {
            y += (h - dim.height) / 2;
            break;
        }
        case RPG_ALIGN_RIGHT:
        case RPG_ALIGN_CENTER_RIGHT: {
            x = w - x - dim.width;
            y += (h - dim.height) / 2;
            break;
        }
        case RPG_ALIGN_NONE:
        case RPG_ALIGN_CENTER: {
            x += (w - dim.width) / 2;
            y += (h - dim.height) / 2;
            break;
        }
    }

    RPGmatrix4x4 ortho;
    MAT4_ORTHO(ortho, 0, w, h, 0, -1.0f, 1.0f);
    glBindFramebuffer(GL_FRAMEBUFFER, fetch_fbo(img));
    glViewport(0, 0, w, h);

    rpg_font_render(font, &ortho, str, x, y);
    UNBIND_BITMAP();
    return self;
}

static VALUE rpg_image_blit(int argc, VALUE *argv, VALUE self) {
    VALUE a1, a2, a3, a4, a5, a6;
    rb_scan_args(argc, argv, "24", &a1, &a2, &a3, &a4, &a5, &a6);

    RPGimage *img = DATA_PTR(self);
    RPGimage *src = DATA_PTR(a1);
    int x, y, w, h;
    float alpha = 1.0f;
    switch (argc) {
        case 2: {
            RPGrect *d = DATA_PTR(a2);
            x = d->x;
            y = d->y;
            w = d->width;
            h = d->height;
            break;
        }
        case 3: {
            if (RB_IS_A(a2, rb_cRect)) {
                RPGrect *d = DATA_PTR(a2);
                x = d->x;
                y = d->y;
                w = d->width;
                h = d->height;
                alpha = rpg_image_value2alpha(a3);
            } else {
                x = NUM2INT(a2);
                y = NUM2INT(a3);
                w = src->width;
                h = src->height;
            }
            break;
        }
        case 5: {
            x = NUM2INT(a2);
            y = NUM2INT(a3);
            w = NUM2INT(a4);
            h = NUM2INT(a5);
            alpha = rpg_image_value2alpha(a5);
            break;
        }
        case 6: {
            x = NUM2INT(a2);
            y = NUM2INT(a3);
            w = NUM2INT(a4);
            h = NUM2INT(a5);
            alpha = rpg_image_value2alpha(a6);
        }
        default: {
            rb_raise(rb_eArgError, "wrong number of arguments (given %d, expected 2, 3, 4, 6)", argc);
            break;
        }
    }

    // Set rendering to this Bitmap's FBO
    glBindFramebuffer(GL_FRAMEBUFFER, fetch_fbo(img));
    RPGmatrix4x4 ortho, model;
    MAT4_ORTHO(ortho, 0.0f, img->width, img->height, 0.0f, -1.0f, 1.0f);
    glUseProgram(_program);
    glUniformMatrix4fv(_projection, 1, GL_FALSE, (float *)&ortho);
    glViewport(0, 0, img->width, img->height);
    glScissor(0, 0, img->width, img->height);

    // Use a simplified model matrix without rotation for blitting
    GLfloat scale_x = (w / (GLfloat)src->width) * src->width;
    GLfloat scale_y = (h / (GLfloat)src->height) * src->height;
    MAT4_SET(model, scale_x, 0.0f, 0.0f, 0.0f, 0.0f, scale_y, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, x, y, 0.0f, 1.0f);
    glUniformMatrix4fv(_model, 1, GL_FALSE, (float *)&model);

    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUniform4f(_color, 0.0f, 0.0f, 0.0f, 0.0f);
    glUniform4f(_tone, 0.0f, 0.0f, 0.0f, 0.0f);
    glUniform1f(_alpha, alpha);
    glUniform4f(_flash, 0.0f, 0.0f, 0.0f, 0.0f);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, src->texture);
    glBindVertexArray(quad_vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    UNBIND_BITMAP();

    return self;
}