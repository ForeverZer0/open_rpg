#ifndef OPEN_RPG_SHADER_H
#define OPEN_RPG_SHADER_H 1

#include "common.h"

void rpg_shader_init(VALUE parent);
static VALUE rpg_shader_alloc(VALUE klass);
static VALUE rpg_shader_initialize(int argc, VALUE *argv, VALUE self);
static VALUE rpg_shader_dispose(VALUE self);
static VALUE rpg_shader_disposed_p(VALUE self);

static VALUE rpg_shader_locate(VALUE self, VALUE value);


static VALUE rpg_shader_from_file(int argc, VALUE *argv, VALUE klass);
#endif /* OPEN_RPG_SHADER_H */