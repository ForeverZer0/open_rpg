#ifndef OPEN_RPG_SHADER_H
#define OPEN_RPG_SHADER_H 1

#include "common.h"

void rpg_shader_init(VALUE parent);
static VALUE rpg_shader_alloc(VALUE klass);

static VALUE rpg_shader_dispose(VALUE self);
static VALUE rpg_shader_disposed_p(VALUE self);
static VALUE rpg_shader_initialize(int argc, VALUE *argv, VALUE self);

GLuint rpg_shader_compile(const char *source, GLenum type);

#endif /* OPEN_RPG_SHADER_H */