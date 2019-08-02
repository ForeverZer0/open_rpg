#ifndef OPEN_RPG_TABLE_H
#define OPEN_RPG_TABLE_H 1

#include "common.h"

void rpg_table_init(VALUE parent);

static VALUE rpg_table_alloc(VALUE klass);
static VALUE rpg_table_initialize(int argc, VALUE *argv, VALUE self);
static VALUE rpg_table_get_value(int argc, VALUE *argv, VALUE self);
static VALUE rpg_table_set_value(int argc, VALUE *argv, VALUE self);
static VALUE rpg_table_resize(int argc, VALUE *argv, VALUE self);
static VALUE rpg_table_width(VALUE self);
static VALUE rpg_table_height(VALUE self);
static VALUE rpg_table_depth(VALUE self);
static VALUE rpg_table_dimensions(VALUE self);

#endif /* OPEN_RPG_TABLE_H */