#ifndef OPEN_RPG_SHADER_H
#define OPEN_RPG_SHADER_H 1

#include "./common.h"

void rpg_shader_init(VALUE parent);
static VALUE rpg_shader_alloc(VALUE klass);
static VALUE rpg_shader_initialize(int argc, VALUE *argv, VALUE self);

static VALUE rpg_shader_locate(VALUE self, VALUE value);
static VALUE rpg_shader_from_file(int argc, VALUE *argv, VALUE klass);

static inline GLuint rpg_create_shader_src(const char *src, GLenum type) {
    if (src == NULL) {
        return 0;
    }
    GLuint shader = glCreateShader(type);
    GLint length = (GLint)strlen(src);
    glShaderSource(shader, 1, &src, &length);
    glCompileShader(shader);
    xfree((void *)src);
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

static inline GLuint rpg_create_shader(const char *fname, GLenum type) {
    if (fname == NULL) {
        return 0;
    }
    GLuint shader = glCreateShader(type);
    size_t len;
    const char *src = rpg_read_file(fname, &len);
    GLint length = (GLint)len;
    glShaderSource(shader, 1, &src, &length);
    glCompileShader(shader);
    xfree((void *)src);
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

#endif /* OPEN_RPG_SHADER_H */