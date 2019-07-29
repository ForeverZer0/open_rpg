#ifndef OPEN_RPG_COMMON_H
#define OPEN_RPG_COMMON_H 1

#include "glad.h"
#include <GLFW/glfw3.h>
#include "ruby.h"

#define uthash_malloc xmalloc
#define uthash_free(ptr,sz) xfree(ptr)
#include "uthash.h"

#include <freetype2/ft2build.h>
#include FT_FREETYPE_H

extern VALUE rb_mOpenRPG;
extern VALUE rb_mDisposable;
extern VALUE rb_mInput;
extern VALUE rb_eRPGError;

extern VALUE rb_cGame;
extern VALUE rb_cBitmap;
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

extern GLint game_width;
extern GLint game_height;
extern GLuint quad_vao;
extern GLuint quad_vbo;

#define STR2SYM(str) ID2SYM(rb_intern(str))
#define NUM2FLT(v) ((GLfloat) NUM2DBL(v))
#define RB_BOOL(exp) ((exp) ? Qtrue : Qfalse)
#define RB_IS_A(obj, klass) rb_obj_is_kind_of(CLASS_OF(obj), klass)
#define FLT_EQL(v1, v2) (fabsf(v1 - v2) < __FLT_EPSILON__)
#define FLT_PI 3.14159274f

#define RECT_SET(_rect, _x, _y, _width, _height)    \
_rect->x = _x;                                      \
_rect->y = _y;                                      \
_rect->width = _width;                              \
_rect->height = _height

#define DUMP_FUNC(function, type)                                   \
static VALUE function(int argc, VALUE *argv, VALUE self) {          \
    void *obj = DATA_PTR(self);                                     \
    return rb_str_new(obj, sizeof(type));                           \
}

#define LOAD_FUNC(function, type)                                   \
static VALUE function(VALUE klass, VALUE str) {                     \
    type *obj = ALLOC(type);                                        \
    void *src = StringValueCStr(str);                               \
    memcpy(obj, src, sizeof(type));                                 \
    return Data_Wrap_Struct(klass, NULL, RUBY_DEFAULT_FREE, obj);   \
}

#define DUP_FUNC(function, type)        \
static VALUE function(VALUE self) {     \
    type *src = DATA_PTR(self);         \
    type *dst = ALLOC(type);            \
    memcpy(dst, src, sizeof(type));     \
    return Data_Wrap_Struct(CLASS_OF(self), NULL, RUBY_DEFAULT_FREE, dst); \
}

#define ATTR_READER(function, type, field, macro)   \
static VALUE function(VALUE self) {                 \
    type *obj = DATA_PTR(self);                     \
    return macro(obj->field);                       \
}

#define ATTR_WRITER(function, type, field, macro)   \
static VALUE function(VALUE self, VALUE value) {    \
    type *obj = DATA_PTR(self);                     \
    obj->field = macro(value);                      \
    return value;                                   \
}

#define ALLOC_FUNC(function, type)                                  \
static VALUE function(VALUE klass) {                                \
    type *value = ALLOC(type);                                      \
    memset(value, 0, sizeof(type));                                 \
    return Data_Wrap_Struct(klass, NULL, RUBY_DEFAULT_FREE, value); \
}

static inline char *rpg_read_file(const char *fname, size_t *length) {
    char *buffer = 0;
    FILE *file = fopen(fname, "rb");
    if (file) {
        fseek(file, 0, SEEK_END);
        long len = ftell(file);
        fseek(file, 0, SEEK_SET);
        buffer = xmalloc(len);
        if (buffer) {
            *length = fread(buffer, 1, len, file);
        }
        fclose(file);
    }
    return buffer;
}

static inline GLuint rpg_create_shader(const char *fname, GLenum type) {
    GLuint shader = glCreateShader(type);
    size_t len;
    const char*src = rpg_read_file(fname, &len);
    GLint length = (GLint) len;
    glShaderSource(shader, 1, &src, &length);
    glCompileShader(shader);
    xfree((void*) src);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success != GL_TRUE) {
        char log[512];
        glGetShaderInfoLog(shader, 512, NULL, log);
        printf(log);
        rb_raise(rb_eRPGError, "failed to compile shader -- %s", log);
    }

    return shader;
}

static inline GLuint rpg_create_shader_program(const char *vert_path, const char *frag_path, const char *geo_path) {
    GLuint program = glCreateProgram();
    GLuint vertex = rpg_create_shader(vert_path, GL_VERTEX_SHADER);
    GLuint fragment = rpg_create_shader(frag_path, GL_FRAGMENT_SHADER);
    GLuint geometry = 0;

    glAttachShader(program, vertex);
    glAttachShader(program, fragment);

    if (geo_path) {
        geometry = rpg_create_shader(geo_path, GL_GEOMETRY_SHADER);
        glAttachShader(program, geometry);
    }

    glLinkProgram(program);

    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (success != GL_TRUE) {
        char log[1024];
        glGetProgramInfoLog(program, 1024, NULL, log);
        rb_raise(rb_eRPGError, "failed to link shader program -- %s", log);
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);
    if (geometry) {
        glDeleteShader(geometry);
    }
 
    return program;
}

static inline int imax(int value, int max) {
    return value < max ? value : max;
}

static inline int imin(int value, int min) {
    return value > min ? value : min;
}

static inline int clampi(int v, int min, int max) {
    return imin(max, imax(min, v));
}

static inline float clampf(float v, float min, float max) {
    return fminf(max, fmaxf(min, v));
}

static inline void check_dimensions(int width, int height) {
    if (width < 1) {
        rb_raise(rb_eArgError, "width cannot be less than 1");
    }
    if (height < 1) {
        rb_raise(rb_eArgError, "height cannot be less than 1");
    }
}

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

typedef struct RPGbatch {
    GLint z;
    int capacity;
    int count;
    void **children;
    GLubyte updated;
} RPGbatch;

typedef struct RPGglyph {
    int codepoint;
    GLuint texture;
    RPGsize size;
    RPGpoint bearing;
    GLint advance;
    UT_hash_handle hh;
} RPGglyph;

typedef struct RPGfont {
    FT_UInt pixel_size;
    RPGcolor *color;
    FT_Face face;
    int v_offset;
    RPGglyph *glyphs;
} RPGfont;

typedef struct RPGbitmap {
    GLint width;
    GLint height;
    GLuint texture;
    GLuint fbo;
    RPGfont *font;
} RPGbitmap;

typedef struct RPGsprite {
    GLint z;
    RPGbitmap *bitmap;
    RPGbatch *group;
    RPGmatrix4x4 *ortho;
    GLfloat alpha;
    GLint x;
    GLint y;
    RPGvector2 scale;
    RPGcolor color;
    RPGtone tone;
    RPGflash flash;
    RPGrotation rotation;
    GLboolean updated;
    GLuint vbo;
    GLuint vao;
    RPGblend blend;
} RPGsprite;

typedef struct RPGgame {
    GLFWwindow *window;
    GLdouble rate;
    GLdouble tick;
    GLuint64 frame_count;
    RPGvector2 ratio;
    RPGrect viewport;
    RPGcolor bg_color;
} RPGgame;

extern GLuint _program;
extern GLint _color;
extern GLint _tone;
extern GLint _alpha;
extern GLint _flash;
extern GLint _depth;
extern GLint _model;
extern GLint _projection;
extern GLint _screen;
extern GLint _screen_z;

#endif /* OPEN_RPG_COMMON_H */