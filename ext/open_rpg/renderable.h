#ifndef OPEN_RPG_RENDERABLE_H
#define OPEN_RPG_RENDERABLE_H 1

#include "./blend.h"
#include "./common.h"

void rpg_renderable_init(VALUE parent);
VALUE rpg_renderable_alloc(VALUE klass);
static VALUE rpg_renderable_update(VALUE self);

static VALUE rpg_renderable_get_alpha(VALUE self);
static VALUE rpg_renderable_set_alpha(VALUE self, VALUE value);
static VALUE rpg_renderable_get_opacity(VALUE self);
static VALUE rpg_renderable_set_opacity(VALUE self, VALUE value);
static VALUE rpg_renderable_get_visible(VALUE self);
static VALUE rpg_renderable_set_visible(VALUE self, VALUE value);
static VALUE rpg_renderable_get_origin(VALUE self);
static VALUE rpg_renderable_set_origin(VALUE self, VALUE value);
static VALUE rpg_renderable_get_ox(VALUE self);
static VALUE rpg_renderable_get_oy(VALUE self);
static VALUE rpg_renderable_get_z(VALUE self);
static VALUE rpg_renderable_set_ox(VALUE self, VALUE value);
static VALUE rpg_renderable_set_oy(VALUE self, VALUE value);
static VALUE rpg_renderable_set_z(VALUE self, VALUE value);
static VALUE rpg_renderable_get_color(VALUE self);
static VALUE rpg_renderable_set_color(VALUE self, VALUE value);
static VALUE rpg_renderable_get_tone(VALUE self);
static VALUE rpg_renderable_set_tone(VALUE self, VALUE value);

static VALUE rpg_renderable_get_scale(VALUE self);
static VALUE rpg_renderable_set_scale(VALUE self, VALUE value);
static VALUE rpg_renderable_zoom(int argc, VALUE *argv, VALUE self);
static VALUE rpg_renderable_get_scale_x(VALUE self);
static VALUE rpg_renderable_set_scale_x(VALUE self, VALUE value);
static VALUE rpg_renderable_get_scale_y(VALUE self);
static VALUE rpg_renderable_set_scale_y(VALUE self, VALUE value);
static VALUE rpg_renderable_get_blend(VALUE self);
static VALUE rpg_renderable_set_blend(VALUE self, VALUE value);

static VALUE rpg_renderable_flash(VALUE self, VALUE color, VALUE duration);
static VALUE rpg_renderable_flash_duration(VALUE self);
static VALUE rpg_renderable_flash_color(VALUE self);
static VALUE rpg_renderable_flashing_p(VALUE self);

static VALUE rpg_renderable_get_angle(VALUE self);
static VALUE rpg_renderable_set_angle(VALUE self, VALUE value);
static VALUE rpg_renderable_get_anchor(VALUE self);
static VALUE rpg_renderable_set_anchor(VALUE self, VALUE value);

static VALUE rpg_renderable_dispose(VALUE self);
static VALUE rpg_renderable_disposed_p(VALUE self);

#define RPG_RENDER_INIT(r)                                                                                                                 \
    r.scale.x = 1.0f;                                                                                                                      \
    r.scale.y = 1.0f;                                                                                                                      \
    r.blend.equation = GL_FUNC_ADD;                                                                                                        \
    r.blend.src_factor = GL_SRC_ALPHA;                                                                                                     \
    r.blend.dst_factor = GL_ONE_MINUS_SRC_ALPHA;                                                                                           \
    r.visible = GL_TRUE;                                                                                                                   \
    r.alpha = 1.0f;                                                                                                                        \
    r.updated = GL_TRUE

#define MAT4_ORTHO(mat4, left, right, top, bottom, near, far)                                                                               \
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

#endif /* OPEN_RPG_RENDERABLE_H */