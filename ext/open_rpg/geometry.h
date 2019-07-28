#ifndef OPEN_RPG_PRIMITIVES_H
#define OPEN_RPG_PRIMITIVES_H 1

#include "common.h"

void rpg_geometry_init(VALUE parent);

static VALUE rpg_point_alloc(VALUE klass);
static VALUE rpg_size_alloc(VALUE klass);
static VALUE rpg_rect_alloc(VALUE klass);

static VALUE rpg_point_initialize(int argc, VALUE *argv, VALUE self);
static VALUE rpg_size_initialize(int argc, VALUE *argv, VALUE self);
static VALUE rpg_rect_initialize(int argc, VALUE *argv, VALUE self);
static VALUE rpg_rect_from_ltrb(VALUE klass, VALUE left, VALUE right, VALUE top, VALUE bottom);

static VALUE rpg_point_set(VALUE self, VALUE x, VALUE y);
static VALUE rpg_size_set(VALUE self, VALUE width, VALUE height);
static VALUE rpg_rect_set(VALUE self, VALUE x, VALUE y, VALUE width, VALUE height);

static VALUE rpg_point_dup(VALUE self);
static VALUE rpg_size_dup(VALUE self);
static VALUE rpg_rect_dup(VALUE self);

static VALUE rpg_point_inspect(VALUE self);
static VALUE rpg_size_inspect(VALUE self);
static VALUE rpg_rect_inspect(VALUE self);

static VALUE rpg_point_get_x(VALUE self);
static VALUE rpg_point_get_y(VALUE self);
static VALUE rpg_point_set_x(VALUE self, VALUE value);
static VALUE rpg_point_set_y(VALUE self, VALUE value);

static VALUE rpg_size_get_width(VALUE self);
static VALUE rpg_size_get_height(VALUE self);
static VALUE rpg_size_set_width(VALUE self, VALUE value);
static VALUE rpg_size_set_height(VALUE self, VALUE value);

static VALUE rpg_rect_get_x(VALUE self);
static VALUE rpg_rect_get_y(VALUE self);
static VALUE rpg_rect_get_width(VALUE self);
static VALUE rpg_rect_get_height(VALUE self);
static VALUE rpg_rect_set_x(VALUE self, VALUE value);
static VALUE rpg_rect_set_y(VALUE self, VALUE value);
static VALUE rpg_rect_set_width(VALUE self, VALUE value);
static VALUE rpg_rect_set_height(VALUE self, VALUE value);
static VALUE rpg_rect_right(VALUE self);
static VALUE rpg_rect_bottom(VALUE self);
static VALUE rpg_rect_area(VALUE self);
static VALUE rpg_rect_perimeter(VALUE self);
static VALUE rpg_rect_location(VALUE self);
static VALUE rpg_rect_size(VALUE self);
static VALUE rpg_rect_include_p(int argc, VALUE *argv, VALUE self);
static VALUE rpg_rect_inflate(int argc, VALUE *argv, VALUE self);
static VALUE rpg_rect_inflate_bang(int argc, VALUE *argv, VALUE self);
static VALUE rpg_rect_intersect(VALUE self, VALUE other);
static VALUE rpg_rect_intersect_bang(VALUE self, VALUE other);
static VALUE rpg_rect_intersects_p(VALUE self, VALUE other);
static VALUE rpg_rect_union(VALUE self, VALUE other);
static VALUE rpg_rect_union_bang(VALUE self, VALUE other);
static VALUE rpg_rect_offset(int argc, VALUE *argv, VALUE self);
static VALUE rpg_rect_offset_bang(int argc, VALUE *argv, VALUE self);

static VALUE rpg_point_equal(VALUE self, VALUE other);
static VALUE rpg_size_equal(VALUE self, VALUE other);
static VALUE rpg_rect_equal(VALUE self, VALUE other);

static VALUE rpg_point_empty_p(VALUE self);
static VALUE rpg_size_empty_p(VALUE self);
static VALUE rpg_rect_empty_p(VALUE self);

static VALUE rpg_point_dump(int argc, VALUE *argv, VALUE self);
static VALUE rpg_size_dump(int argc, VALUE *argv, VALUE self);
static VALUE rpg_rect_dump(int argc, VALUE *argv, VALUE self);
static VALUE rpg_point_load(VALUE klass, VALUE binary);
static VALUE rpg_size_load(VALUE klass, VALUE binary);
static VALUE rpg_rect_load(VALUE klass, VALUE binary);

static VALUE rpg_point_add(VALUE self, VALUE other);
static VALUE rpg_point_minus(VALUE self, VALUE other);
static VALUE rpg_size_add(VALUE self, VALUE other);
static VALUE rpg_size_minus(VALUE self, VALUE other);
static VALUE rpg_point_offset(int argc, VALUE *argv, VALUE self);
static VALUE rpg_point_offset_bang(int argc, VALUE *argv, VALUE self);

#endif /* OPEN_RPG_PRIMITIVES_H */