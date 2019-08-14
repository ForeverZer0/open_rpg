#include "./common.h"
#include "./font.h"

VALUE rb_cImage;

GLuint blit_vbo;
GLuint blit_vao;

#define JPEG_QUALITY 95

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#define STBI_MALLOC RPG_ALLOC
#define STBI_REALLOC RPG_REALLOC
#define STBI_FREE RPG_FREE
#define STBIW_MALLOC STBI_MALLOC
#define STBIW_REALLOC STBI_REALLOC
#define STBIW_FREE STBI_FREE

#define BYTES_PER_PIXEL 4

#include "./stb_image.h"
#include "./stb_image_write.h"

#define BIND_FRAMEBUFFER(_img, _x, _y, _w, _h)                                                                                             \
    glBindFramebuffer(GL_FRAMEBUFFER, fetch_fbo(_img));                                                                                    \
    RPGmatrix4x4 _m;                                                                                                                       \
    MAT4_ORTHO(_m, 0.0f, _w, 0.0f, _h, -1.0f, 1.0f);                                                                                       \
    glUseProgram(_program);                                                                                                                \
    glUniformMatrix4fv(_projection, 1, GL_FALSE, (float *)&_m);                                                                            \
    glViewport(_x, _y, _w, _h);                                                                                                            \
    glScissor(_x, _y, _w, _h)

#define UNBIND_FRAMEBUFFER()                                                                                                             \
    glBindFramebuffer(GL_FRAMEBUFFER, 0);                                                                                                  \
    glUseProgram(_program);                                                                                                                \
    glUniformMatrix4fv(_projection, 1, GL_FALSE, (float *)&projection);                                                                    \
    glViewport(bounds.x, bounds.y, bounds.width, bounds.height);                                                                           \
    glScissor(bounds.x, bounds.y, bounds.width, bounds.height)

ATTR_READER(rpg_image_fbo, RPGimage, fbo, UINT2NUM)
ATTR_READER(rpg_image_texture, RPGimage, texture, UINT2NUM)

static VALUE rpg_image_dispose(VALUE self) {
    struct RData *rdata = RDATA(self);
    RPGimage *img = rdata->data;
    if (img->texture) {
        glDeleteTextures(1, &img->texture);
    }
    if (img->fbo) {
        glDeleteFramebuffers(1, &img->fbo);
    }
    RPG_FREE(img);
    rdata->data = NULL;
}

static VALUE rpg_image_disposed_p(VALUE self) {
    return RB_BOOL(DATA_PTR(self) == NULL);
}

void *rpg_image_load(const char *fname, int *width, int *height) {
    if (FILE_EXISTS(fname)) {
        return stbi_load(fname, width, height, NULL, 4);
    }
    VALUE e = rb_const_get(rb_mErrno, rb_intern("ENOENT"));
    rb_raise(e, "%s", fname);
}

static inline GLuint fetch_fbo(RPGimage *img) {
    if (img->fbo == 0) {
        if (img->texture == 0) {
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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
    return texture;
}

static inline void rpg_image_fill_inline(RPGimage *img, int x, int y, int width, int height, RPGcolor *color) {
    BIND_FRAMEBUFFER(img, x, y, width, height);
    glClearColor(color->r, color->g, color->b, color->a);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(bg_color.r, bg_color.g, bg_color.b, bg_color.a);
    UNBIND_FRAMEBUFFER();
}

static VALUE rpg_image_alloc(VALUE klass) {
    RPGimage *img = ALLOC(RPGimage);
    memset(img, 0, sizeof(RPGimage));
    return Data_Wrap_Struct(klass, NULL, NULL, img);
}

void *rpg_image_pixels(RPGimage *image, int *size) {
    *size = BYTES_PER_PIXEL * image->width * image->height;
    void *pixels = RPG_ALLOC(*size);
    BIND_FRAMEBUFFER(image, 0, 0, image->width, image->height);
    glReadPixels(0, 0, image->width, image->height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    UNBIND_FRAMEBUFFER();
    return pixels;
}

static VALUE rpg_image_blob(VALUE self) {
    RPGimage *image = DATA_PTR(self);
    int size;
    void *pixels = rpg_image_pixels(image, &size);
    return rb_str_new(pixels, (long)size);
}

static VALUE rpg_image_save(VALUE self, VALUE path, VALUE format) {
    stbi_flip_vertically_on_write(GL_TRUE);
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
    RPG_FREE(pixels);
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
    return Data_Wrap_Struct(rb_cSize, NULL, NULL, size);
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

static VALUE rpg_image_from_file(VALUE klass, VALUE path) {
    const char *fname = StringValueCStr(path);
    RPGimage *img = ALLOC(RPGimage);
    if (!FILE_EXISTS(fname)) {
        VALUE e = rb_const_get(rb_mErrno, rb_intern("ENOENT"));
        rb_raise(e, "\"%s\"", fname);
    }

    void *pixels = stbi_load(fname, &img->width, &img->height, NULL, 4);
    if (pixels == NULL) {
        rb_raise(rb_eRPGError, "failed to load image - %s", fname);
    }
    img->font = NULL;
    img->fbo = 0;
    img->texture = rpg_image_generate(img->width, img->height, pixels, GL_RGBA);
    RPG_FREE(pixels);

    return Data_Wrap_Struct(klass, NULL, NULL, img);
}

static VALUE rpg_image_initialize(int argc, VALUE *argv, VALUE self) {

    VALUE a1, a2, a3;
    rb_scan_args(argc, argv, "21", &a1, &a2, &a3);
    RPGimage *img = DATA_PTR(self);

    img->width = NUM2INT(a1);
    img->height = NUM2INT(a2);
    check_dimensions(img->width, img->height);

    if (RTEST(a3)) {
        size_t size = img->width * img->height;
        void *pixels = RPG_ALLOC(size * BYTES_PER_PIXEL);

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
        img->texture = rpg_image_generate(img->width, img->height, pixels, GL_RGBA);
        RPG_FREE(pixels);
    } else {
        img->texture = rpg_image_generate(img->width, img->height, NULL, GL_RGBA);
    }
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

    return Data_Wrap_Struct(klass, NULL, NULL, img);
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
    BIND_FRAMEBUFFER(img, 0, 0, img->width, img->height);
    glReadPixels(x, y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &c);
    UNBIND_FRAMEBUFFER();

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
    RPGcolor color = { 0.0f, 0.0f, 0.0f, 0.0f };
    rpg_image_fill_inline(img, 0, 0, img->width, img->height, &color);
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

    void *pixels = RPG_ALLOC(BYTES_PER_PIXEL * w * h);
    BIND_FRAMEBUFFER(src, x, y, w, h);
    glReadPixels(x, y, w, h, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    UNBIND_FRAMEBUFFER();

    dst->texture = rpg_image_generate(w, h, pixels, GL_RGBA);
    RPG_FREE(pixels);

    return Data_Wrap_Struct(CLASS_OF(self), NULL, NULL, dst);
}

static VALUE rpg_image_get_font(VALUE self) {
    RPGimage *image = DATA_PTR(self);
    if (image->font) {
        return Data_Wrap_Struct(rb_cFont, NULL, RUBY_DEFAULT_FREE, image->font);
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

static VALUE rpg_image_draw_text(int argc, VALUE *argv, VALUE self) {

    VALUE a1, a2, a3, a4, a5, a6;
    rb_scan_args(argc, argv, "24", &a1, &a2, &a3, &a4, &a5, &a6);

    RPGimage *img = DATA_PTR(self);
    RPGfont *font = img->font == NULL ? &default_font : img->font;

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
            y = h - y - dim.height - (font->size / 3);
            break;
        }
        case RPG_ALIGN_BOTTOM_RIGHT: {
            x = w - x - dim.width;
            y = h - y - dim.height - (font->size / 3);
            break;
        }
        case RPG_ALIGN_BOTTOM:
        case RPG_ALIGN_BOTTOM_CENTER: {
            x += (w - dim.width) / 2;
            y = h - y - dim.height - (font->size / 3);
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
    MAT4_ORTHO(ortho, 0, img->width, img->height, 0, -1.0f, 1.0f);
    glBindFramebuffer(GL_FRAMEBUFFER, fetch_fbo(img));
    glViewport(0, 0, img->width, img->height);
    glScissor(0, 0, img->width, img->height);
    rpg_font_render(font, &ortho, str, x, y);
    UNBIND_FRAMEBUFFER();
    return self;
}

static inline VALUE rpg_image_blit(int argc, VALUE *argv, VALUE self) {
    VALUE a1, a2, a3, a4, a5, a6, a7;
    rb_scan_args(argc, argv, "34", &a1, &a2, &a3, &a4, &a5, &a6, &a7);

    RPGimage *img = DATA_PTR(self);
    RPGimage *src = DATA_PTR(a1);
    RPGrect *s = DATA_PTR(a2);
    float alpha = 1.0f;
    int dx, dy, dw, dh;

    switch (argc) {
        case 3: {
            int *d = DATA_PTR(a3);
            dx = d[0];
            dy = d[1];
            if (RB_IS_A(a3, rb_cRect)) {
                dw = d[2];
                dh = d[3];
            } else {
                dw = src->width;
                dy = src->height;
            }
            break;
        }
        case 4: {
            if (FIXNUM_P(a3)) {
                dx = NUM2INT(a3);
                dy = NUM2INT(a4);
                dw = src->width;
                dy = src->height;
            } else {
                int *d = DATA_PTR(a3);
                dx = d[0];
                dy = d[1];
                if (RB_IS_A(a3, rb_cRect)) {
                    dw = d[2];
                    dh = d[3];
                } else {
                    dw = src->width;
                    dy = src->height;
                }
                alpha = rpg_image_value2alpha(a4);
            }
            break;
        }
        case 5: {
            dx = NUM2INT(a3);
            dy = NUM2INT(a4);
            dw = src->width;
            dy = src->height;
            alpha = rpg_image_value2alpha(a5);
            break;
        }
        case 6:
        case 7: {
            dx = NUM2INT(a3);
            dy = NUM2INT(a4);
            dw = NUM2INT(a5);
            dh = NUM2INT(a6);
            if (argc == 7) {
                alpha = rpg_image_value2alpha(a7);
            }
            break;
        }
    }

    if (blit_vao == 0) {
        glGenVertexArrays(1, &blit_vao);
        glGenBuffers(1, &blit_vbo);
        glBindVertexArray(blit_vao);
        glBindBuffer(GL_ARRAY_BUFFER, blit_vbo);
        glBufferData(GL_ARRAY_BUFFER, VERTICES_SIZE, NULL, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), NULL);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    // Set rendering to this Bitmap's FBO
    glBindFramebuffer(GL_FRAMEBUFFER, fetch_fbo(img));
    RPGmatrix4x4 ortho;
    MAT4_ORTHO(ortho, 0.0f, img->width, img->height, 0.0f, -1.0f, 1.0f);
    glUseProgram(_program);
    glUniformMatrix4fv(_projection, 1, GL_FALSE, (float *)&ortho);
    glViewport(0, 0, img->width, img->height);
    glScissor(0, 0, img->width, img->height);

    GLfloat scale_x = (dw / (GLfloat)s->width) * s->width;
    GLfloat scale_y = (dh / (GLfloat)s->height) * s->height;
    RPGmatrix4x4 model;
    MAT4_SET(model, scale_x, 0.0f, 0.0f, 0.0f, 0.0f, scale_y, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, dx, dy, 0.0f, 1.0f);

    // Set shader uniforms for opacity and ortho
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glUniformMatrix4fv(_model, 1, GL_FALSE, (float *)&model);
    glUniform1f(_alpha, alpha);
    glUniform4f(_color, 0.0f, 0.0f, 0.0f, 0.0f);
    glUniform4f(_tone, 0.0f, 0.0f, 0.0f, 0.0f);
    glUniform4f(_flash, 0.0f, 0.0f, 0.0f, 0.0f);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(blit_vao);

    GLfloat l = (GLfloat) s->x / src->width;
    GLfloat t = (GLfloat) s->y / src->height;
    GLfloat r = l + ((GLfloat) s->width /  src->width);
    GLfloat b = t + ((GLfloat) s->height / src->height);
    glBindBuffer(GL_ARRAY_BUFFER, blit_vbo);
    float vertices[VERTICES_COUNT] = {
        0.0f, 1.0f, l, b, 
        1.0f, 0.0f, r, t, 
        0.0f, 0.0f, l, t,
        0.0f, 1.0f, l, b, 
        1.0f, 1.0f, r, b, 
        1.0f, 0.0f, r, t
    };
    glBufferSubData(GL_ARRAY_BUFFER, 0, VERTICES_SIZE, vertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindTexture(GL_TEXTURE_2D, src->texture);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    UNBIND_FRAMEBUFFER();
    return self;
}

void rpg_image_init(VALUE parent) {
    rb_cImage = rb_define_class_under(parent, "Image", rb_cObject);
    rb_define_alloc_func(rb_cImage, rpg_image_alloc);
    rb_define_method(rb_cImage, "initialize", rpg_image_initialize, -1);
    rb_define_method(rb_cImage, "dispose", rpg_image_dispose, 0);
    rb_define_method(rb_cImage, "disposed?", rpg_image_disposed_p, 0);

    rb_define_method(rb_cImage, "width", rpg_image_width, 0);
    rb_define_method(rb_cImage, "height", rpg_image_height, 0);
    rb_define_method(rb_cImage, "size", rpg_image_size, 0);
    rb_define_method(rb_cImage, "rect", rpg_image_rect, 0);
    rb_define_method(rb_cImage, "pixels", rpg_image_blob, 0);

    rb_define_alias(rb_cImage, "columns", "width");
    rb_define_alias(rb_cImage, "rows", "height");
    rb_define_alias(rb_cImage, "to_blob", "pixels");

    rb_define_method(rb_cImage, "get_pixel", rpg_image_get_pixel, -1);
    rb_define_method(rb_cImage, "set_pixel", rpg_image_set_pixel, -1);
    rb_define_method(rb_cImage, "fill_rect", rpg_image_fill_rect, -1);
    rb_define_method(rb_cImage, "blit", rpg_image_blit, -1);

    rb_define_method(rb_cImage, "slice", rpg_image_slice, -1);
    rb_define_method(rb_cImage, "clear", rpg_image_clear, 0);
    rb_define_method(rb_cImage, "save", rpg_image_save, 2);

    rb_define_method(rb_cImage, "font", rpg_image_get_font, 0);
    rb_define_method(rb_cImage, "font=", rpg_image_set_font, 1);
    rb_define_method(rb_cImage, "draw_text", rpg_image_draw_text, -1);

    rb_define_method(rb_cImage, "fbo", rpg_image_fbo, 0);
    rb_define_method(rb_cImage, "texture", rpg_image_texture, 0);

    rb_define_singleton_method(rb_cImage, "from_blob", rpg_image_from_blob, -1);
    rb_define_singleton_method(rb_cImage, "from_file", rpg_image_from_file, 1);

    rb_define_const(rb_cImage, "FORMAT_PNG", INT2NUM(RPG_FORMAT_PNG));
    rb_define_const(rb_cImage, "FORMAT_JPG", INT2NUM(RPG_FORMAT_JPG));
    rb_define_const(rb_cImage, "FORMAT_BMP", INT2NUM(RPG_FORMAT_BMP));

    rb_define_const(rb_cImage, "TYPE_RGBA", INT2NUM(GL_RGBA));
    rb_define_const(rb_cImage, "TYPE_RGB", INT2NUM(GL_RGB));
    rb_define_const(rb_cImage, "TYPE_BGRA", INT2NUM(GL_BGRA));
    rb_define_const(rb_cImage, "TYPE_BGR", INT2NUM(GL_BGR));

    // TODO: Define in open_rpg.c
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