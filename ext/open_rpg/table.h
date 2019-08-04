#ifndef OPEN_RPG_TABLE_H
#define OPEN_RPG_TABLE_H 1

#include "./common.h"

void rpg_table_init(VALUE parent);
void rpg_table_free(void *data);

static VALUE rpg_table_alloc(VALUE klass);
static VALUE rpg_table_initialize(int argc, VALUE *argv, VALUE self);
static VALUE rpg_table_get_value(int argc, VALUE *argv, VALUE self);
static VALUE rpg_table_set_value(int argc, VALUE *argv, VALUE self);
static VALUE rpg_table_resize(int argc, VALUE *argv, VALUE self);
static VALUE rpg_table_width(VALUE self);
static VALUE rpg_table_height(VALUE self);
static VALUE rpg_table_depth(VALUE self);
static VALUE rpg_table_dimensions(VALUE self);
static VALUE rpg_table_clear(VALUE self);
static VALUE rpg_table_each(VALUE self);
static VALUE rpg_table_length(VALUE self);
static VALUE rpg_table_enum_length(VALUE ary, VALUE args, VALUE eobj);
static VALUE rpg_table_dup(VALUE self);
static VALUE rpg_table_inspect(VALUE self);

#endif /* OPEN_RPG_TABLE_H */