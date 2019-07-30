#ifndef OPEN_RPG_MATH_H
#define OPEN_RPG_MATH_H 1

#include "./common.h"

void rpg_mat4_create_ortho(RPGmatrix4x4 *mat4, float left, float right, float bottom, float top, float near, float far);

void rpg_numerics_init(VALUE parent);

static VALUE rpg_vec2_alloc(VALUE klass);
static VALUE rpg_vec3_alloc(VALUE klass);
static VALUE rpg_vec4_alloc(VALUE klass);
static VALUE rpg_quat_alloc(VALUE klass);
static VALUE rpg_mat3_alloc(VALUE klass);
static VALUE rpg_mat4_alloc(VALUE klass);

static VALUE rpg_vec2_dup(VALUE self);
static VALUE rpg_vec3_dup(VALUE self);
static VALUE rpg_vec4_dup(VALUE self);
static VALUE rpg_quat_dup(VALUE self);
static VALUE rpg_mat3_dup(VALUE self);
static VALUE rpg_mat4_dup(VALUE self);

static VALUE rpg_vec2_equal(VALUE self, VALUE other);
static VALUE rpg_vec3_equal(VALUE self, VALUE other);
static VALUE rpg_vec4_equal(VALUE self, VALUE other);
static VALUE rpg_quat_equal(VALUE self, VALUE other);
static VALUE rpg_mat3_equal(VALUE self, VALUE other);
static VALUE rpg_mat4_equal(VALUE self, VALUE other);

static VALUE rpg_vec2_inspect(VALUE self);
static VALUE rpg_vec3_inspect(VALUE self);
static VALUE rpg_vec4_inspect(VALUE self);
static VALUE rpg_quat_inspect(VALUE self);
static VALUE rpg_mat3_inspect(VALUE self);
static VALUE rpg_mat4_inspect(VALUE self);

static VALUE rpg_vec2_initialize(int argc, VALUE *argv, VALUE self);
static VALUE rpg_vec3_initialize(int argc, VALUE *argv, VALUE self);
static VALUE rpg_vec4_initialize(int argc, VALUE *argv, VALUE self);
static VALUE rpg_mat3_initialize(int argc, VALUE *argv, VALUE self);
static VALUE rpg_mat4_initialize(int argc, VALUE *argv, VALUE self);

static VALUE rpg_vec2_dump(int argc, VALUE *argv, VALUE self);
static VALUE rpg_vec3_dump(int argc, VALUE *argv, VALUE self);
static VALUE rpg_vec4_dump(int argc, VALUE *argv, VALUE self);
static VALUE rpg_quat_dump(int argc, VALUE *argv, VALUE self);
static VALUE rpg_mat3_dump(int argc, VALUE *argv, VALUE self);
static VALUE rpg_mat4_dump(int argc, VALUE *argv, VALUE self);

static VALUE rpg_vec2_load(VALUE klass, VALUE binary);
static VALUE rpg_vec3_load(VALUE klass, VALUE binary);
static VALUE rpg_vec4_load(VALUE klass, VALUE binary);
static VALUE rpg_quat_load(VALUE klass, VALUE binary);
static VALUE rpg_mat3_load(VALUE klass, VALUE binary);
static VALUE rpg_mat4_load(VALUE klass, VALUE binary);

static VALUE rpg_vec2_each(VALUE self);
static VALUE rpg_vec3_each(VALUE self);
static VALUE rpg_vec4_each(VALUE self);
static VALUE rpg_quat_each(VALUE self);
static VALUE rpg_mat3_each(VALUE self);
static VALUE rpg_mat4_each(VALUE self);

static VALUE rpg_vec2_get_x(VALUE self);
static VALUE rpg_vec2_get_y(VALUE self);
static VALUE rpg_vec2_set_x(VALUE self, VALUE value);
static VALUE rpg_vec2_set_y(VALUE self, VALUE value);

static VALUE rpg_vec3_get_x(VALUE self);
static VALUE rpg_vec3_get_y(VALUE self);
static VALUE rpg_vec3_get_z(VALUE self);
static VALUE rpg_vec3_set_x(VALUE self, VALUE value);
static VALUE rpg_vec3_set_y(VALUE self, VALUE value);
static VALUE rpg_vec3_set_z(VALUE self, VALUE value);

static VALUE rpg_vec4_get_x(VALUE self);
static VALUE rpg_vec4_get_y(VALUE self);
static VALUE rpg_vec4_get_z(VALUE self);
static VALUE rpg_vec4_get_w(VALUE self);
static VALUE rpg_vec4_set_x(VALUE self, VALUE value);
static VALUE rpg_vec4_set_y(VALUE self, VALUE value);
static VALUE rpg_vec4_set_z(VALUE self, VALUE value);
static VALUE rpg_vec4_set_w(VALUE self, VALUE value);

static VALUE rpg_quat_get_x(VALUE self);
static VALUE rpg_quat_get_y(VALUE self);
static VALUE rpg_quat_get_z(VALUE self);
static VALUE rpg_quat_get_w(VALUE self);
static VALUE rpg_quat_set_x(VALUE self, VALUE value);
static VALUE rpg_quat_set_y(VALUE self, VALUE value);
static VALUE rpg_quat_set_z(VALUE self, VALUE value);
static VALUE rpg_quat_set_w(VALUE self, VALUE value);

static VALUE rpg_mat3_get_m11(VALUE self);
static VALUE rpg_mat3_get_m12(VALUE self);
static VALUE rpg_mat3_get_m21(VALUE self);
static VALUE rpg_mat3_get_m22(VALUE self);
static VALUE rpg_mat3_get_m31(VALUE self);
static VALUE rpg_mat3_get_m32(VALUE self);
static VALUE rpg_mat3_set_m11(VALUE self, VALUE value);
static VALUE rpg_mat3_set_m12(VALUE self, VALUE value);
static VALUE rpg_mat3_set_m21(VALUE self, VALUE value);
static VALUE rpg_mat3_set_m22(VALUE self, VALUE value);
static VALUE rpg_mat3_set_m31(VALUE self, VALUE value);
static VALUE rpg_mat3_set_m32(VALUE self, VALUE value);

static VALUE rpg_mat4_get_m11(VALUE self);
static VALUE rpg_mat4_get_m12(VALUE self);
static VALUE rpg_mat4_get_m13(VALUE self);
static VALUE rpg_mat4_get_m14(VALUE self);
static VALUE rpg_mat4_get_m21(VALUE self);
static VALUE rpg_mat4_get_m22(VALUE self);
static VALUE rpg_mat4_get_m23(VALUE self);
static VALUE rpg_mat4_get_m24(VALUE self);
static VALUE rpg_mat4_get_m31(VALUE self);
static VALUE rpg_mat4_get_m32(VALUE self);
static VALUE rpg_mat4_get_m33(VALUE self);
static VALUE rpg_mat4_get_m34(VALUE self);
static VALUE rpg_mat4_get_m41(VALUE self);
static VALUE rpg_mat4_get_m42(VALUE self);
static VALUE rpg_mat4_get_m43(VALUE self);
static VALUE rpg_mat4_get_m44(VALUE self);
static VALUE rpg_mat4_set_m11(VALUE self, VALUE value);
static VALUE rpg_mat4_set_m12(VALUE self, VALUE value);
static VALUE rpg_mat4_set_m13(VALUE self, VALUE value);
static VALUE rpg_mat4_set_m14(VALUE self, VALUE value);
static VALUE rpg_mat4_set_m21(VALUE self, VALUE value);
static VALUE rpg_mat4_set_m22(VALUE self, VALUE value);
static VALUE rpg_mat4_set_m23(VALUE self, VALUE value);
static VALUE rpg_mat4_set_m24(VALUE self, VALUE value);
static VALUE rpg_mat4_set_m31(VALUE self, VALUE value);
static VALUE rpg_mat4_set_m32(VALUE self, VALUE value);
static VALUE rpg_mat4_set_m33(VALUE self, VALUE value);
static VALUE rpg_mat4_set_m34(VALUE self, VALUE value);
static VALUE rpg_mat4_set_m41(VALUE self, VALUE value);
static VALUE rpg_mat4_set_m42(VALUE self, VALUE value);
static VALUE rpg_mat4_set_m43(VALUE self, VALUE value);
static VALUE rpg_mat4_set_m44(VALUE self, VALUE value);

#endif /* OPEN_RPG_MATH_H */