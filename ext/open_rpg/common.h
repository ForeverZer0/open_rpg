#ifndef OPEN_RPG_COMMON_H
#define OPEN_RPG_COMMON_H 1

#include "glad.h"
#include "ruby.h"
#include <GLFW/glfw3.h>
#include <freetype2/ft2build.h>

#define uthash_malloc xmalloc
#define uthash_free(ptr, sz) xfree(ptr)
#include "uthash.h"

extern VALUE rb_mOpenRPG;
extern VALUE rb_mInput;
extern VALUE rb_mGraphics;
extern VALUE rb_mApplication;
extern VALUE rb_mGame;
extern VALUE rb_eRPGError;
extern VALUE rb_cRenderable;
extern VALUE rb_cSprite;
extern VALUE rb_cBlend;
extern VALUE rb_cViewport;
extern VALUE rb_cImage;
extern VALUE rb_cFont;
extern VALUE rb_cPoint;
extern VALUE rb_cSize;
extern VALUE rb_cRect;
extern VALUE rb_cColor;
extern VALUE rb_cTone;

// Numerics
extern VALUE rb_cVector2;
extern VALUE rb_cVector3;
extern VALUE rb_cVector4;
extern VALUE rb_cQuaternion;
extern VALUE rb_cMatrix3x2;
extern VALUE rb_cMatrix4x4;

#define VERTICES_COUNT 24
#define VERTICES_SIZE (sizeof(float) * VERTICES_COUNT)
#define VERTICES_STRIDE (sizeof(float) * 4)

#define FILE_EXISTS(fname) (access((fname), F_OK) != -1)
#define STR2SYM(str) ID2SYM(rb_intern(str))
#define NUM2FLT(v) ((GLfloat)NUM2DBL(v))
#define RB_BOOL(exp) ((exp) ? Qtrue : Qfalse)
#define RB_IS_A(obj, klass) (rb_obj_is_kind_of(obj, klass) == Qtrue)
#define FLT_EQL(v1, v2) (fabsf(v1 - v2) < __FLT_EPSILON__)
#define FLT_PI 3.14159274f

#define RECT_SET(_rect, _x, _y, _width, _height)                                                                                           \
    _rect->x = _x;                                                                                                                         \
    _rect->y = _y;                                                                                                                         \
    _rect->width = _width;                                                                                                                 \
    _rect->height = _height

#define DUMP_FUNC(function, type)                                                                                                          \
    static VALUE function(int argc, VALUE *argv, VALUE self) {                                                                             \
        void *obj = DATA_PTR(self);                                                                                                        \
        return rb_str_new(obj, sizeof(type));                                                                                              \
    }

#define LOAD_FUNC(function, type)                                                                                                          \
    static VALUE function(VALUE klass, VALUE str) {                                                                                        \
        type *obj = ALLOC(type);                                                                                                           \
        void *src = StringValueCStr(str);                                                                                                  \
        memcpy(obj, src, sizeof(type));                                                                                                    \
        return Data_Wrap_Struct(klass, NULL, RUBY_DEFAULT_FREE, obj);                                                                      \
    }

#define DUP_FUNC(function, type)                                                                                                           \
    static VALUE function(VALUE self) {                                                                                                    \
        type *src = DATA_PTR(self);                                                                                                        \
        type *dst = ALLOC(type);                                                                                                           \
        memcpy(dst, src, sizeof(type));                                                                                                    \
        return Data_Wrap_Struct(CLASS_OF(self), NULL, RUBY_DEFAULT_FREE, dst);                                                             \
    }

#define ATTR_READER(function, type, field, macro)                                                                                          \
    static VALUE function(VALUE self) {                                                                                                    \
        type *obj = DATA_PTR(self);                                                                                                        \
        return macro(obj->field);                                                                                                          \
    }

#define ATTR_WRITER(function, type, field, macro)                                                                                          \
    static VALUE function(VALUE self, VALUE value) {                                                                                       \
        type *obj = DATA_PTR(self);                                                                                                        \
        obj->field = macro(value);                                                                                                         \
        return value;                                                                                                                      \
    }

#define ALLOC_FUNC(function, type)                                                                                                         \
    static VALUE function(VALUE klass) {                                                                                                   \
        type *value = ALLOC(type);                                                                                                         \
        memset(value, 0, sizeof(type));                                                                                                    \
        return Data_Wrap_Struct(klass, NULL, RUBY_DEFAULT_FREE, value);                                                                    \
    }

static inline int imax(int value, int max) { return value > max ? value : max; }

static inline int imin(int value, int min) { return value < min ? value : min; }

static inline int clampi(int v, int min, int max) { return imin(max, imax(min, v)); }

static inline float clampf(float v, float min, float max) { return fminf(max, fmaxf(min, v)); }

static inline void check_dimensions(int width, int height) {
    if (width < 1) {
        rb_raise(rb_eArgError, "width cannot be less than 1");
    }
    if (height < 1) {
        rb_raise(rb_eArgError, "height cannot be less than 1");
    }
}

static inline char *rpg_expand_path_s(VALUE str) {
    VALUE absolute = rb_file_s_absolute_path(1, &str);
    return StringValueCStr(absolute);
}

static inline char *rpg_expand_path(const char *str) {
    VALUE s = rb_str_new2(str);
    return rpg_expand_path_s(s);
}

typedef void (*RPGrenderfunc)(void *renderable);

typedef struct RPGpoint {
    GLint x;
    GLint y;
} RPGpoint;

typedef struct RPGsize {
    GLint width;
    GLint height;
} RPGsize;

typedef struct RPGrect {
    GLint x;
    GLint y;
    GLint width;
    GLint height;
} RPGrect;

typedef struct RPGcolor {
    GLfloat r;
    GLfloat g;
    GLfloat b;
    GLfloat a;
} RPGcolor;

typedef struct RPGtone {
    GLfloat r;
    GLfloat g;
    GLfloat b;
    GLfloat gr;
} RPGtone;

typedef struct RPGvector2 {
    GLfloat x;
    GLfloat y;
} RPGvector2;

typedef struct RPGvector3 {
    GLfloat x;
    GLfloat y;
    GLfloat z;
} RPGvector3;

typedef struct RPGvector4 {
    GLfloat x;
    GLfloat y;
    GLfloat z;
    GLfloat w;
} RPGvector4;

typedef struct RPGmatrix3x2 {
    GLfloat m11;
    GLfloat m12;
    GLfloat m21;
    GLfloat m22;
    GLfloat m31;
    GLfloat m32;
} RPGmatrix3x2;

typedef struct RPGmatrix4x4 {
    GLfloat m11;
    GLfloat m12;
    GLfloat m13;
    GLfloat m14;
    GLfloat m21;
    GLfloat m22;
    GLfloat m23;
    GLfloat m24;
    GLfloat m31;
    GLfloat m32;
    GLfloat m33;
    GLfloat m34;
    GLfloat m41;
    GLfloat m42;
    GLfloat m43;
    GLfloat m44;
} RPGmatrix4x4;

typedef struct RPGshader {
    GLuint program;
    GLuint vertex;
    GLuint fragment;
    GLuint geometry;
} RPGshader;

typedef struct RPGblend {
    GLenum equation;
    GLenum src_factor;
    GLenum dst_factor;
} RPGblend;

typedef struct RPGrotation {
    GLfloat radians;
    GLint ox;
    GLint oy;
} RPGrotation;

typedef struct RPGflash {
    RPGcolor color;
    GLubyte duration;
} RPGflash;

typedef struct RPGimage {
    GLint width;
    GLint height;
    GLuint texture;
    GLuint fbo;
    void *font;
} RPGimage;

typedef struct RPGrenderable {
    GLint z;
    GLint ox;
    GLint oy;
    GLboolean updated;
    GLboolean visible;
    GLfloat alpha;
    RPGcolor color;
    RPGtone tone;
    RPGflash flash;
    RPGvector2 scale;
    RPGrotation rotation;
    RPGblend blend;
    RPGrenderfunc render;
    RPGmatrix4x4 model;
    GLubyte disposed;
} RPGrenderable;

typedef struct RPGbatch {
    RPGrenderable **items;
    int capacity;
    int total;
    GLboolean updated;
} RPGbatch;

typedef struct RPGviewport {
    RPGrenderable base;
    RPGrect rect;
    RPGbatch *batch;
    GLuint fbo;
    GLuint texture;
    RPGmatrix4x4 projection;
} RPGviewport;

typedef struct RPGsprite {
    RPGrenderable base;
    RPGimage *image;
    RPGviewport *viewport;
    RPGrect src_rect;
    GLint x;
    GLint y;
    GLuint vbo;
    GLuint vao;
    GLfloat hue;
} RPGsprite;

typedef struct RPGwindow_template {

} RPGwindow_template;

typedef struct RPGwindow {
    RPGrenderable base;
    RPGrect rect;
    RPGwindow_template *template;
    RPGimage *windowskin;
    RPGimage *contents;

    GLuint texture;
    GLuint fbo;
    GLubyte invalidated;
} RPGwindow;

typedef struct RPGfont {
    ID path;
    GLuint size;
    RPGcolor color;
} RPGfont;

typedef struct RPGtable {
    GLbyte dims;
    int width;
    int height;
    int depth;
    short *data;
} RPGtable;

extern GLFWwindow *game_window;
extern RPGbatch *game_batch;
extern RPGfont default_font;

extern VALUE ENOENT;
extern GLint game_width;
extern GLint game_height;
extern ID render_id;
extern GLuint quad_vao;
extern GLuint quad_vbo;
extern RPGcolor bg_color;
extern RPGmatrix4x4 projection;
extern RPGrect bounds;
extern GLuint _program;
extern GLint _color;
extern GLint _tone;
extern GLint _alpha;
extern GLint _flash;
extern GLint _hue;
extern GLint _model;
extern GLint _projection;

char *rpg_read_file(const char *fname, size_t *length);

void rpg_shader_free(void *data);
void rpg_table_free(void *data);


#endif /* OPEN_RPG_COMMON_H */