#ifndef OPEN_RPG_COMMON_H
#define OPEN_RPG_COMMON_H 1

#define RPG_ALLOC ruby_xmalloc
#define RPG_FREE ruby_xfree
#define RPG_REALLOC ruby_xrealloc
#define RPG_PI 3.14159274f
#define MAX_FRAME_RATE 1000
#define MIN_FRAME_RATE 1
#define DEFAULT_FRAME_RATE 40
#define STOCK_VERTEX_SHADER "stock.vert"
#define STOCK_FRAGMENT_SHADER "stock.frag"

#include "./config.h"
#include "./glad.h"
#include "ruby.h"
#include <GLFW/glfw3.h>
#include <freetype2/ft2build.h>

#define uthash_malloc RPG_ALLOC
#define uthash_free(ptr, sz) RPG_FREE(ptr)
#include "uthash.h"

/**
 * @brief The top-level OpenRPG module.
 */
extern VALUE rb_mOpenRPG;

extern VALUE rb_mInput;
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
extern VALUE rb_cWindow;
extern VALUE rb_cWindowskin;

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

#define RB_RESPOND_TO(v, id) rb_obj_respond_to(v, id, 0)
#define FILE_EXISTS(fname) (access((fname), F_OK) != -1)
#define STR2SYM(str) ID2SYM(rb_intern(str))
#define NUM2FLT(v) ((GLfloat)NUM2DBL(v))
#define RB_BOOL(exp) ((exp) ? Qtrue : Qfalse)
#define RB_IS_A(obj, klass) (rb_obj_is_kind_of(obj, klass) == Qtrue)
#define FLT_EQL(v1, v2) (fabsf(v1 - v2) < __FLT_EPSILON__)

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

/**
 * @brief Sets the values of a 4x4 matrix
 */
#define MAT4_ORTHO(mat4, left, right, top, bottom, near, far)                                                                              \
    mat4.m11 = 2.0f / (right - left);                                                                                                      \
    mat4.m12 = mat4.m13 = mat4.m14 = 0.0f;                                                                                                 \
    mat4.m22 = 2.0f / (top - bottom);                                                                                                      \
    mat4.m21 = mat4.m23 = mat4.m24 = 0.0f;                                                                                                 \
    mat4.m33 = 1.0f / (near - far);                                                                                                        \
    mat4.m31 = mat4.m32 = mat4.m34 = 0.0f;                                                                                                 \
    mat4.m41 = (left + right) / (GLfloat)(left - right);                                                                                   \
    mat4.m42 = (top + bottom) / (GLfloat)(bottom - top);                                                                                   \
    mat4.m43 = near / (GLfloat)(near - far);                                                                                               \
    mat4.m44 = 1.0f

/**
 * @brief Sets the values of a 4x4 matrix.
 */
#define MAT4_SET(_mat, _m11, _m12, _m13, _m14, _m21, _m22, _m23, _m24, _m31, _m32, _m33, _m34, _m41, _m42, _m43, _m44)                     \
    _mat.m11 = _m11;                                                                                                                       \
    _mat.m12 = _m12;                                                                                                                       \
    _mat.m13 = _m13;                                                                                                                       \
    _mat.m14 = _m14;                                                                                                                       \
    _mat.m21 = _m21;                                                                                                                       \
    _mat.m22 = _m22;                                                                                                                       \
    _mat.m23 = _m23;                                                                                                                       \
    _mat.m24 = _m24;                                                                                                                       \
    _mat.m31 = _m31;                                                                                                                       \
    _mat.m32 = _m32;                                                                                                                       \
    _mat.m33 = _m33;                                                                                                                       \
    _mat.m34 = _m34;                                                                                                                       \
    _mat.m41 = _m41;                                                                                                                       \
    _mat.m42 = _m42;                                                                                                                       \
    _mat.m43 = _m43;                                                                                                                       \
    _mat.m44 = _m44

/**
 * @brief Prototype function called when a render is required for an object.
 *
 * @param renderable Address of a base renderable structure, typically will be cast to the actual type.
 */
typedef void (*RPGrenderfunc)(void *renderable);

/**
 * @brief Describes a two-dimensinal location in Euclidean coordinates.
 */
typedef struct RPGpoint {
    GLint x; /** The location on the x-axis. */
    GLint y; /** The location on the y-axis. */
} RPGpoint;

/**
 * @brief Describes the size of a two-dimensional object.
 */
typedef struct RPGsize {
    GLint width;  /** The dimension on the x-axis. */
    GLint height; /** The dimension on the y-axis. */
} RPGsize;

/**
 * @brief Describes a shape with four sides and four 90 degree angles.
 */
typedef struct RPGrect {
    GLint x;      /** The location on the x-axis. */
    GLint y;      /** The location on the y-axis. */
    GLint width;  /** The dimension on the x-axis. */
    GLint height; /** The dimension on the y-axis. */
} RPGrect;

/**
 * @brief Describes a color in the RGBA colorspace.
 */
typedef struct RPGcolor {
    GLfloat r; /** Value of the red component in range of 0.0 to 1.0. */
    GLfloat g; /** Value of the green component in range of 0.0 to 1.0. */
    GLfloat b; /** Value of the blue component in range of 0.0 to 1.0. */
    GLfloat a; /** Value of the alpha component in range of 0.0 to 1.0. */
} RPGcolor;

/**
 * @brief Describes an amount of to add/remove from a color, and grayscaling factor.
 */
typedef struct RPGtone {
    GLfloat r;  /** The amount of the red component to change in range of -1.0 to 1.0.  */
    GLfloat g;  /** The amount of the green component to change in range of -1.0 to 1.0.  */
    GLfloat b;  /** The amount of the blue component to change in range of -1.0 to 1.0.  */
    GLfloat gr; /** The amount of grayscaling to apply in range of 0.0 to 1.0.  */
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
    GLfloat hue;
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
    RPGrenderable base; /** The base renderable object, MUST BE FIRST FIELD IN THE STRUCTURE! */
    RPGrect rect;
    RPGbatch *batch;
    GLuint fbo;
    GLuint texture;
    RPGmatrix4x4 projection;
} RPGviewport;

/**
 * @brief Specialized sprite that automatically tiles its source image across its bounds.
 */
typedef struct RPGplane {
    RPGrenderable base;    /** The base renderable object, MUST BE FIRST FIELD IN THE STRUCTURE! */
    RPGimage *image;       /** A pointer ot the sprite's image, or NULL. */
    RPGviewport *viewport; /** A pointer to the sprite's viewport, or NULL. */
    RPGrect rect;          /** Rectangle describing the plane's on-screen location and size. */
    GLuint vbo;            /** The Vertex Buffer Object bound to this sprite. */
    GLuint vao;            /** The Vertex Array Object bound to this sprite. */
    GLboolean update_vao;  /** Flag indicating the plane's VAO needs updated to reflect a change. */
    RPGvector2 zoom;       /** The amount of scaling to apply to the source image. */
} RPGplane;

/**
 * @brief Contains the information required to render an arbitrary image on-screen.
 */
typedef struct RPGsprite {
    RPGrenderable base;    /** The base renderable object, MUST BE FIRST FIELD IN THE STRUCTURE! */
    RPGimage *image;       /** A pointer ot the sprite's image, or NULL. */
    RPGviewport *viewport; /** A pointer to the sprite's viewport, or NULL. */
    GLint x;               /** The location of the sprite on the x-axis. */
    GLint y;               /** The location of the sprite on the y-axis. */
    RPGrect src_rect;      /** The source rectangle of the sprite's image. */
    GLuint vbo;            /** The Vertex Buffer Object bound to this sprite. */
    GLuint vao;            /** The Vertex Array Object bound to this sprite. */
} RPGsprite;

/**
 * @brief Contains information required to draw a font.
 */
typedef struct RPGfont {
    ID path;                /** The ID of an interned string containing the the absolute path to the font file. */
    GLuint size;            /** The size, in points, of the font. */
    RPGcolor color;         /** The foreground color used to render the text. */
    GLboolean outline;      /** Flag indicating if outline should be drawn around text. */
    RPGcolor outline_color; /** The color used to render the outline of the text. */
} RPGfont;

/**
 * @brief A multi-dimensional array with signed 16-bit integer values..
 */
typedef struct RPGtable {
    GLbyte dims; /** The number of dimensions in the table, either 1, 2, or 3. */
    int width;   /** The number of elements on the x-axis. */
    int height;  /** The number of elements on the y-axis. */
    int depth;   /** The number of elements on the z-axis. */
    short *data; /** A pointer to the elements. */
} RPGtable;

typedef enum {
    RPG_NONE = 0x00,

    RPG_LEFT = 0x01,
    RPG_TOP = 0x02,
    RPG_RIGHT = 0x04,
    RPG_BOTTOM = 0x08,
    RPG_TOP_LEFT = RPG_TOP | RPG_LEFT,
    RPG_TOP_RIGHT = RPG_TOP | RPG_RIGHT,
    RPG_BOTTOM_LEFT = RPG_BOTTOM | RPG_LEFT,
    RPG_BOTTOM_RIGHT = RPG_BOTTOM | RPG_RIGHT,

    RPG_WEST = RPG_LEFT,
    RPG_NORTH = RPG_TOP,
    RPG_EAST = RPG_RIGHT,
    RPG_SOUTH = RPG_BOTTOM,
    RPG_NORTH_WEST = RPG_TOP_LEFT,
    RPG_NORTH_EAST = RPG_TOP_RIGHT,
    RPG_SOUTH_WEST = RPG_BOTTOM_LEFT,
    RPG_SOUTH_EAST = RPG_BOTTOM_RIGHT,

    RPG_ALL_DIRECTIONS = 0xFF
} RPGdirection;

enum RPGimageformat { RPG_FORMAT_PNG, RPG_FORMAT_JPG, RPG_FORMAT_BMP };

extern GLFWwindow *game_window;
extern RPGbatch *game_batch;
extern RPGfont default_font;

extern GLint game_width;
extern GLint game_height;
extern GLfloat game_ratio_x;
extern GLfloat game_ratio_y;
extern GLint screen_width;
extern GLint screen_height;
extern GLdouble game_tick;

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

/**
 * @brief Returns the largest of the two given values.
 *
 * @param a First value.
 * @param b Second value.
 * @return int The largest of the two values.
 */
inline int imax(int a, int b) { return a > b ? a : b; }

/**
 * @brief Returns the smaller of the two given values.
 *
 * @param a First value.
 * @param b Second value.
 * @return int The smaller of the two values.
 */
inline int imin(int a, int b) { return a < b ? a : b; }

/**
 * @brief Clamps the specified value, guaranteeing it to be between an upper and lower range.
 *
 * @param v The value to clamp.
 * @param min The minimum possible value this function will return.
 * @param max The maximum possible value this function will return.
 * @return float The value, or the min/max if value was out of range.
 */
inline int clampi(int v, int min, int max) { return imin(max, imax(min, v)); }

/**
 * @brief Clamps the specified value, guaranteeing it to be between an upper and lower range.
 *
 * @param v The value to clamp.
 * @param min The minimum possible value this function will return.
 * @param max The maximum possible value this function will return.
 * @return float The value, or the min/max if value was out of range.
 */
inline float clampf(float v, float min, float max) { return fminf(max, fmaxf(min, v)); }

/**
 * @brief Checks that the specified dimensions are each greater than 0, or raises an ArgumentError in Ruby if not.
 *
 * @param width The width value to check.
 * @param height The height value to check.
 */
inline void check_dimensions(int width, int height) {
    if (width < 1) {
        rb_raise(rb_eArgError, "width cannot be less than 1");
    }
    if (height < 1) {
        rb_raise(rb_eArgError, "height cannot be less than 1");
    }
}

/**
 * @brief Loads a supported image file.
 *
 * @param fname The path to the file to load.
 * @param width The location to write the width of the image, in pixels.
 * @param height The location to write the height of the image, in pixels.
 * @return void* A pointer to the pixels of the image, where each pixel is in RGBA order, 8-bits per component. This
 *      buffer needs freed when it is no longer needed.
 */
void *rpg_image_load(const char *fname, int *width, int *height);

/**
 * @brief Reads the pixels of an RPGimage and returns copies it to a buffer.
 *
 * @param image The image to read the pixels of.
 * @param size Location where the number of bytes in the returned buffer will be written.
 * @return void* A pointer to the image pixels. This pointer needs freed when it is no longer needed.
 */
void *rpg_image_pixels(RPGimage *image, int *size);

/**
 * @brief Reads the contents of a file and returns it.
 *
 * @param fname The path of the file to read.
 * @param length A location to write the number of bytes in the returned buffer.
 * @return char* The contents of the file, or NULL if an error occured.
 */
char *rpg_read_file(const char *fname, size_t *length);

/**
 * @brief Creates a shader from the given source code.
 *
 * @param src A buffer containng the source code for the shader.
 * @param type The type of shader.
 * @return GLuint The created shader.
 */
GLuint rpg_create_shader_src(const char *src, GLenum type);

/**
 * @brief Creates a shader from the specified source file.
 *
 * @param fname The path to the shader source code.
 * @param type The type of shader.
 * @return GLuint The created shader.
 */
GLuint rpg_create_shader(const char *fname, GLenum type);

/**
 * @brief Creates a new GLSL shader program.
 *
 * @param vert_path The path to the source file for the vertex shader.
 * @param frag_path The path to the source file for ther fragment shader.
 * @param geo_path The path to the source file for ther geometry shader, or NULL to not use.
 * @return GLuint The created shader program.
 */
GLuint rpg_create_shader_program(const char *vert_path, const char *frag_path, const char *geo_path);

#endif /* OPEN_RPG_COMMON_H */