/**
 * @file rpg.h
 * @author Eric "ForeverZer0" Freed
 * @brief Contains definitions for all publicly exposed features of the API.
 * @version 0.1
 * @date 2019-08-14
 *
 * @copyright Copyright (c) 2019
 */
#ifndef OPEN_RPG_COMMON_H
#define OPEN_RPG_COMMON_H 1

/**
 * @brief The "malloc" function used by the library and all libraries that link to it.
 */
#define RPG_ALLOC ruby_xmalloc

/**
 * @brief The "free" function used by the library and all libraries that link to it.
 */
#define RPG_FREE ruby_xfree

/**
 * @brief The "realloc" function used by the library and all libraries that link to it.
 */
#define RPG_REALLOC ruby_xrealloc

/**
 * @brief PI as a single-precision float.
 */
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

/**
 * @brief The OpenRPG::Input module.
 */
extern VALUE rb_mInput;

/**
 * @brief The OpenRPG::Game module.
 */
extern VALUE rb_mGame;

/**
 * @brief The OpenRPG::RPGError exception class.
 */
extern VALUE rb_eRPGError;

/**
 * @brief The OpenRPG::Point class.
 */
extern VALUE rb_cPoint;

/**
 * @brief The OpenRPG::Size class.
 */
extern VALUE rb_cSize;

/**
 * @brief The OpenRPG::Rect class.
 */
extern VALUE rb_cRect;

/**
 * @brief The OpenRPG::Color class.
 */
extern VALUE rb_cColor;

/**
 * @brief The OpenRPG::Tone class.
 */
extern VALUE rb_cTone;

/**
 * @brief The OpenRPG::Vector2 class.
 */
extern VALUE rb_cVec2;

/**
 * @brief The OpenRPG::Vector3 class.
 */
extern VALUE rb_cVec3;

/**
 * @brief The OpenRPG::Vector4 class.
 */
extern VALUE rb_cVec4;

/**
 * @brief The OpenRPG::Matrix3x3 class.
 */
extern VALUE rb_cMat3;

/**
 * @brief The OpenRPG::Matrix4x4 class.
 */
extern VALUE rb_cMat4;

/**
 * @brief The OpenRPG::Image class.
 */
extern VALUE rb_cImage;

/**
 * @brief The base OpenRPG::Renderable class.
 */
extern VALUE rb_cRenderable;


extern VALUE rb_mApplication; // TODO: Don't think these need exposed publicly
extern VALUE rb_cSprite;
extern VALUE rb_cSpriteSheet;
extern VALUE rb_cBlend;
extern VALUE rb_cViewport;
extern VALUE rb_cFont;
extern VALUE rb_cWindow;


/**
 * @brief Analog of Ruby's "respond_to?" method.
 */
#define RB_RESPOND_TO(v, id) rb_obj_respond_to(v, id, 0)

/**
 * @brief Checks if the specified file exists.
 */
#define RPG_FILE_EXISTS(fname) (access((fname), F_OK) != -1)

/**
 * @brief Converts a C char* to a Ruby symbol VALUE.
 */
#define STR2SYM(str) ID2SYM(rb_intern(str))

/**
 * @brief Converts a Ruby VALUE to a single-precision float.
 */
#define NUM2FLT(v) ((GLfloat)NUM2DBL(v))

/**
 * @brief Evaluates expression to either Qtrue or Qfalse.
 */
#define RB_BOOL(exp) ((exp) ? Qtrue : Qfalse)

/**
 * @brief Analog of Ruby's "is_a?" method.
 */
#define RB_IS_A(obj, klass) (rb_obj_is_kind_of(obj, klass) == Qtrue)

/**
 * @brief Compares equality of single-precision floats using an epsilon.
 */
#define FLT_EQL(a, b) (fabsf(a - b) < __FLT_EPSILON__)

/**
 * @brief Resets the clear color back to the user-defined value.
 */
#define RPG_RESET_BACK_COLOR() glClearColor(rpgBACK_COLOR.r, rpgBACK_COLOR.g, rpgBACK_COLOR.b, rpgBACK_COLOR.a)

/**
 * @brief Sets the viewport and scissor rectangle of the primary viewport.
 */
#define RPG_VIEWPORT(x, y, w, h)                                                                                                           \
    glViewport(x, y, w, h);                                                                                                                \
    glScissor(x, y, w, h)

/**
 * @brief Resets the primary viewport to fit the window correctly.
 */
#define RPG_RESET_VIEWPORT() RPG_VIEWPORT(rpgBOUNDS.x, rpgBOUNDS.y, rpgBOUNDS.width, rpgBOUNDS.height)

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
#define MAT4_SET(mat4, _m11, _m12, _m13, _m14, _m21, _m22, _m23, _m24, _m31, _m32, _m33, _m34, _m41, _m42, _m43, _m44)                     \
    mat4.m11 = _m11;                                                                                                                       \
    mat4.m12 = _m12;                                                                                                                       \
    mat4.m13 = _m13;                                                                                                                       \
    mat4.m14 = _m14;                                                                                                                       \
    mat4.m21 = _m21;                                                                                                                       \
    mat4.m22 = _m22;                                                                                                                       \
    mat4.m23 = _m23;                                                                                                                       \
    mat4.m24 = _m24;                                                                                                                       \
    mat4.m31 = _m31;                                                                                                                       \
    mat4.m32 = _m32;                                                                                                                       \
    mat4.m33 = _m33;                                                                                                                       \
    mat4.m34 = _m34;                                                                                                                       \
    mat4.m41 = _m41;                                                                                                                       \
    mat4.m42 = _m42;                                                                                                                       \
    mat4.m43 = _m43;                                                                                                                       \
    mat4.m44 = _m44

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

/**
 * @brief A structure encapsulating two single precision floating point values
 */
typedef struct RPGvec2 {
    GLfloat x; /** The value of the x component. */
    GLfloat y; /** The value of the y component. */
} RPGvec2;

/**
 * @brief A structure encapsulating three single precision floating point values
 */
typedef struct RPGvec3 {
    GLfloat x; /** The value of the x component. */
    GLfloat y; /** The value of the y component. */
    GLfloat z; /** The value of the z component. */
} RPGvec3;

/**
 * @brief A structure encapsulating four single precision floating point values.
 */
typedef struct RPGvec4 {
    GLfloat x; /** The value of the x component. */
    GLfloat y; /** The value of the y component. */
    GLfloat z; /** The value of the z component. */
    GLfloat w; /** The value of the w component. */
} RPGvec4;

/**
 * @brief A structure encapsulating a 3x3 matrix.
 */
typedef struct RPGmat3 {
    GLfloat m11; /** Value at row 1, column 1 of the matrix. */
    GLfloat m12; /** Value at row 1, column 2 of the matrix. */
    GLfloat m13; /** Value at row 1, column 3 of the matrix. */
    GLfloat m21; /** Value at row 2, column 1 of the matrix. */
    GLfloat m22; /** Value at row 2, column 2 of the matrix. */
    GLfloat m23; /** Value at row 2, column 3 of the matrix. */
    GLfloat m31; /** Value at row 3, column 1 of the matrix. */
    GLfloat m32; /** Value at row 3, column 2 of the matrix. */
    GLfloat m33; /** Value at row 3, column 3 of the matrix. */
} RPGmat3;

/**
 * @brief A structure encapsulating a 4x4 matrix.
 */
typedef struct RPGmat4 {
    GLfloat m11; /** Value at row 1, column 1 of the matrix. */
    GLfloat m12; /** Value at row 1, column 2 of the matrix. */
    GLfloat m13; /** Value at row 1, column 3 of the matrix. */
    GLfloat m14; /** Value at row 1, column 4 of the matrix. */
    GLfloat m21; /** Value at row 2, column 1 of the matrix. */
    GLfloat m22; /** Value at row 2, column 2 of the matrix. */
    GLfloat m23; /** Value at row 2, column 3 of the matrix. */
    GLfloat m24; /** Value at row 2, column 4 of the matrix. */
    GLfloat m31; /** Value at row 3, column 1 of the matrix. */
    GLfloat m32; /** Value at row 3, column 2 of the matrix. */
    GLfloat m33; /** Value at row 3, column 3 of the matrix. */
    GLfloat m34; /** Value at row 3, column 4 of the matrix. */
    GLfloat m41; /** Value at row 4, column 1 of the matrix. */
    GLfloat m42; /** Value at row 4, column 2 of the matrix. */
    GLfloat m43; /** Value at row 4, column 3 of the matrix. */
    GLfloat m44; /** Value at row 4, column 4 of the matrix. */
} RPGmat4;

/**
 * @brief A GLSL shader program.
 */
typedef struct RPGshader {
    GLuint program; /** The program name suitable for OpenGL functions. */
} RPGshader;

typedef struct RPGblend {
    GLenum equation;   /** Value indicatiing the blending equation to use. */
    GLenum src_factor; /** Value indicating the source pixel factor. */
    GLenum dst_factor; /** Value indicating the destination pixel factor. */
} RPGblend;

/**
 * @brief Describes the rotation to be applied to a sprite.
 */
typedef struct RPGrotation {
    GLfloat radians; /** The amount of rotation, in radians. */
    GLint ox;        /** The anchor point on the x-axis to rotate around, relative to the sprite. */
    GLint oy;        /** The anchor point on the y-axis to rotate around, relative to the sprite. */
} RPGrotation;

/**
 * @brief Describes a flash effect of a sprite.
 */
typedef struct RPGflash {
    RPGcolor color;   /** The color to use for the flash effect. */
    GLubyte duration; /** The number of remaining frames to apply the flash effect. */
} RPGflash;

/**
 * @brief Represents an OpenGL texture and FBO.
 */
typedef struct RPGimage {
    GLint width;    /** The size of the image on the x-axis, in pixels. */
    GLint height;   /** The size of the image on the x-axis, in pixels. */
    GLuint texture; /** The texture of the image. */
    GLuint fbo;     /** The FBO of the image, created on when needed. */
    void *font;     /** A pointer to a font to use for text on this image, or NULL to use default font. */
} RPGimage;

/**
 * @brief Base structure for objects that can be rendered. MUST BE FIRST FIELD IN!
 */
typedef struct RPGrenderable {
    GLint z;              /** The position of the object on the z-axis. */
    GLint ox;             /** The origin point on the x-axis, which has implementation-dependent meaning. */
    GLint oy;             /** The origin point on the y-axis, which has implementation-dependent meaning. */
    GLboolean updated;    /** Flag indicating if the model matrix needs updated to reflect changes. */
    GLboolean visible;    /** Flag indicating if object should be rendered. */
    GLfloat alpha;        /** The opacity level to be rendered at in the range of 0.0 to 1.0. */
    RPGcolor color;       /** The color to blended when rendered. */
    RPGtone tone;         /** The tone to apply when rendered. */
    GLfloat hue;          /** The amount of hue to apply, in degrees. */
    RPGflash flash;       /** The flash effect to apply when rendered. */
    RPGvec2 scale;        /** The amount of scale to apply when rendered. */
    RPGrotation rotation; /** The amount of rotation and anchor to apply when rendered. */
    RPGblend blend;       /** The blending factors to apply during rendering. */
    RPGrenderfunc render; /** The function to call when the object needs rendered. */
    RPGmat4 model;        /** The model matrix for the object. */
    GLboolean disposed;   /** Flag indicating if object has been disposed. */
} RPGrenderable;

/**
 * @brief Container for a rendering batch, with a quick-sort based on sprite's position on the z-axis.
 */
typedef struct RPGbatch {
    RPGrenderable **items; /** An array of pointers to the sprites within this batch. */
    int capacity;          /** The total capacity the batch can hold before reallocation. */
    int total;             /** The total number of sprites within the batch. */
    GLboolean updated;     /** Flag indicating the items may need reordered due to added entry or change of z-axis. */
} RPGbatch;

/**
 * @brief A container for sprites that is drawn in its own independent batch with its own projection.
 */
typedef struct RPGviewport {
    RPGrenderable base; /** The base renderable object, MUST BE FIRST FIELD IN THE STRUCTURE! */
    RPGrect rect;       /** Rectangle describing the plane's on-screen location and size. */
    RPGbatch *batch;    /** A collection containing pointers to the sprites within this viewport. */
    GLuint fbo;         /** The Framebuffer Object for rendering to the viewport. */
    GLuint texture;     /** Texture with the viewport batch rendered onto it. */
    RPGmat4 projection; /** The projection matrix for the viewport. */
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
    RPGvec2 zoom;          /** The amount of scaling to apply to the source image. */
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
 * @brief Optimized sprite with pre-defined vertex arrays for tiled source images.
 */
typedef struct RPGspritesheet {
    RPGsprite sprite;   /** The base sprite structure. */
    RPGsize cellsize;   /** The size of the cells, in pixels. */
    RPGpoint cellcount; /** Total number of cells (i.e. vertex arrays) */
    RPGpoint current;   /** Current selected tile. */
    GLuint *vbos;       /** An array of buffers, one for each tile. */
    GLuint *vaos;       /** An array of vertex arrays, ont for each tile. */
} RPGspritesheet;

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

/**
 * @brief Bitfields describing directions.
 */
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

/**
 * @brief Image formats that images be be saved with.
 */
enum RPGimageformat { RPG_FORMAT_PNG, RPG_FORMAT_JPG, RPG_FORMAT_BMP };

/**
 * @brief A handle used for the native game window.
 */
typedef GLFWwindow RPGwindow;

/**
 * @brief Global game window instance.
 */
extern RPGwindow *rpgGAME_WINDOW;

/**
 * @brief Top-level batch, containing references to all renderable objects.
 */
extern RPGbatch *rpgRENDER_BATCH;

/**
 * @brief The default font instance, used as a fallback when an image has not defined font, and as a prototype for new fonts.
 */
extern RPGfont rpgDEFAULT_FONT;

/**
 * @brief The size of the game's internal resolution on the x-axis, in pixels.
 */
extern GLint rpgWIDTH;

/**
 * @brief The size of the game's internal resolution on the y-axis, in pixels.
 */
extern GLint rpgHEIGHT;

/**
 * @brief The ratio on the x-axis between the internal resolution and the size of the window.
 */
extern GLfloat rpgRATIO_X;

/**
 * @brief The ratio on the y-axis between the internal resolution and the size of the window.
 */
extern GLfloat rpgRATIO_Y;

/**
 * @brief The time, in milliseconds, between game updates, calculated from the user-defined frame rate.
 */
extern GLdouble rpgTICK;

/**
 * @brief The background or "clear" color, seen when nothing is rendered on screen.
 */
extern RPGcolor rpgBACK_COLOR;

/**
 * @brief The projection matrix used for displaying the game on the screen.
 */
extern RPGmat4 rpgPROJECTION;

/**
 * @brief A rectangle describing the area within the window where the game is rendered to.
 */
extern RPGrect rpgBOUNDS;

/**
 * @brief The default shader program used by OpenRPG for rendering.
 */
extern GLuint rpgPROGRAM;

/**
 * @brief The location of the "color" uniform in the default shader program.
 */
extern GLint rpgUNIFORM_COLOR;

/**
 * @brief The location of the "tone" uniform in the default shader program.
 */
extern GLint rpgUNIFORM_TONE;

/**
 * @brief The location of the "alpha" uniform in the default shader program.
 */
extern GLint rpgUNIFORM_ALPHA;

/**
 * @brief The location of the "flash" uniform in the default shader program.
 */
extern GLint rpgUNIFORM_FLASH;

/**
 * @brief The location of the "hue" uniform in the default shader program.
 */
extern GLint rpgUNIFORM_HUE;

/**
 * @brief The location of the "model" uniform in the default shader program.
 */
extern GLint rpgUNIFORM_MODEL;

/**
 * @brief The location of the "projection" uniform in the default shader program.
 */
extern GLint rpgUNIFORM_PROJECTION;

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