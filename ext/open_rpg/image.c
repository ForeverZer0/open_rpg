#include "image.h"
#include <unistd.h>

VALUE rb_cImage;

#define STB_IMAGE_IMPLEMENTATION
#define STBI_MALLOC xmalloc
#define STBI_REALLOC xrealloc
#define STBI_FREE xfree
#include "stb_image.h"

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

    rb_define_method(rb_cImage, "get_pixel", rpg_image_get_pixel, -1);
    rb_define_method(rb_cImage, "set_pixel", rpg_image_set_pixel, -1);
    rb_define_method(rb_cImage, "fill", rpg_image_fill, -1);
    rb_define_method(rb_cImage, "clear", rpg_image_clear, 0);
}

static inline GLuint rpg_image_fbo(RPGimage *img) {
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

static inline GLuint rpg_image_generate(int width, int height, void* pixels, int format) {
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
    rpg_image_bind_fbo(img, x, y, width, height);
    glClearColor(color->r, color->g, color->b, color->a);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    rpg_image_unbind_fbo();
}

void rpg_image_bind_fbo(RPGimage *img, int x, int y, int width, int height) {
    GLuint fbo = rpg_image_fbo(img);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(x, y, width, height);
    glScissor(x, y, width, height);
}

void rpg_image_unbind_fbo(void) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    int x, y, width, height;
    GLFWwindow *window = glfwGetCurrentContext();
    RPGgame *game = glfwGetWindowUserPointer(window);
    glViewport(game->viewport.x, game->viewport.y, game->viewport.width, game->viewport.height);
    glScissor(game->viewport.x, game->viewport.y, game->viewport.width, game->viewport.height);
}

void* rpg_image_load(const char *fname, int *width, int *height) {
    return stbi_load(fname, width, height, NULL, 4);
}

void rpg_image_free(void *data) {
    if (data) {
        RPGimage *img = data;
        if (img->texture) {
            glDeleteTextures(1, &img->texture);
        }
        if (img->fbo) {
            glDeleteFramebuffers(1, &img->fbo);
        }
    }
    xfree(data);
}

VALUE rpg_image_alloc(VALUE klass) {
    RPGimage *img = ALLOC(RPGimage);
    memset(img, 0, sizeof(RPGimage));
    return Data_Wrap_Struct(klass, NULL, rpg_image_free, img);
}

static VALUE rpg_image_dispose(VALUE self) {
    RPGimage *img = DATA_PTR(self);
    if (img->texture) {
        glDeleteTextures(1, &img->texture);
        img->texture = 0;
    }
    if (img->fbo) {
        glDeleteFramebuffers(1, &img->fbo);
        img->fbo = 0;
    }
}

static VALUE rpg_image_disposed_p(VALUE self) {
    RPGimage *img = DATA_PTR(self);
    return img->texture ? Qfalse : Qtrue;
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

static VALUE rpg_image_initialize(int argc, VALUE *argv, VALUE self) {

    VALUE arg1, arg2, arg3;
    rb_scan_args(argc, argv, "12", &arg1, &arg2, &arg3);

    RPGimage *image = DATA_PTR(self);
    void* pixels;

    if (argc == 1) {
        Check_Type(arg1, T_STRING);
        const char *fname = StringValueCStr(arg1);
        if (access(fname, F_OK) == -1) {
            VALUE enoent = rb_const_get(rb_mErrno, rb_intern("ENOENT"));
            rb_raise(enoent, "Cannot find file: %s", fname);
        }
        pixels = stbi_load(fname, &image->width, &image->height, NULL, BYTES_PER_PIXEL);
    }
    else {
        image->width = NUM2INT(arg1);
        image->height = NUM2INT(arg2);
        check_dimensions(image->width, image->height);
        int size = image->width * image->height;
        pixels = xmalloc(size * BYTES_PER_PIXEL);

        if (RTEST(arg3)) {
            RPGcolor *color = DATA_PTR(arg3);
            GLuint *data = (GLuint*) pixels;
            
            GLubyte r = (GLubyte)(color->r * 255);
            GLubyte g = (GLubyte)(color->g * 255);
            GLubyte b = (GLubyte)(color->b * 255);
            GLubyte a = (GLubyte)(color->a * 255);
            GLuint value = (r << 0) | (g << 8) | (b << 16) | (a << 24);

            for (int i = 0; i < size; i++) {
                data[i] = value;
            }
        }
        else {
            memset(pixels, 0, size * BYTES_PER_PIXEL);
        }
    }

    image->texture = rpg_image_generate(image->width, image->height, pixels, GL_RGBA);
    xfree(pixels);
    return Qnil;
}

static VALUE rpg_image_get_pixel(int argc, VALUE *argv, VALUE self) {
    VALUE a1, a2;
    rb_scan_args(argc, argv, "11", &a1, &a2);
    int x, y;
    if (argc == 2) {
        x = NUM2INT(a1);
        y = NUM2INT(a2);
    }
    else {
        RPGpoint *point = DATA_PTR(a1);
        x = point->x;
        y = point->y;
    }

    RPGimage *img = DATA_PTR(self);
    if (x < 0 || x >= img->width || y < 0 || y >= img->height) {
        rb_raise(rb_eArgError, "location is outside of image");
    }

    GLuint c;
    rpg_image_bind_fbo(img, 0, 0, img->width, img->height);
    glReadPixels(x, y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &c);
    rpg_image_unbind_fbo();

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
    }
    else {
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

static VALUE rpg_image_fill(int argc, VALUE *argv, VALUE self) {
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