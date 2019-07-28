#include "shader.h"

VALUE rb_cShader;

void rpg_shader_init(VALUE parent) {
    rb_cShader = rb_define_class_under(parent, "Shader", rb_cObject);

    rb_define_alloc_func(rb_cShader, rpg_shader_alloc);

    rb_define_method(rb_cShader, "initialize", rpg_shader_initialize, -1);
    rb_define_method(rb_cShader, "dispose", rpg_shader_dispose, 0);
    rb_define_method(rb_cShader, "disposed?", rpg_shader_disposed_p, 0);
}

ALLOC_FUNC(rpg_shader_alloc, RPGshader)

static VALUE rpg_shader_dispose(VALUE self) {
    RPGshader *shader = DATA_PTR(self);
    if (shader->geometry) {
        glDeleteShader(shader->geometry);
        shader->geometry = 0;
    }
    if (shader->fragment) {
        glDeleteShader(shader->fragment);
        shader->fragment = 0;
    }
    if (shader->vertex) {
        glDeleteShader(shader->vertex);
        shader->vertex = 0;
    }
    if (shader->program) {
        glDeleteProgram(shader->program);
        shader->program = 0;
    }
    return Qnil;
}

static VALUE rpg_shader_disposed_p(VALUE self) {
    RPGshader *shader = DATA_PTR(self);
    return shader->program ? Qfalse : Qtrue;
}

static VALUE rpg_shader_initialize(int argc, VALUE *argv, VALUE self) {
    VALUE vert_src, frag_src, geo_src;
    rb_scan_args(argc, argv, "21", &vert_src, &frag_src, &geo_src);

    RPGshader *shader = DATA_PTR(self);
    shader->program = glCreateProgram();

    shader->vertex = rpg_shader_compile(StringValueCStr(vert_src), GL_VERTEX_SHADER);
    glAttachShader(shader->program, shader->vertex);

    shader->fragment = rpg_shader_compile(StringValueCStr(frag_src), GL_FRAGMENT_SHADER);
    glAttachShader(shader->program, shader->fragment);

    if (RTEST(geo_src)) {
        shader->geometry = rpg_shader_compile(StringValueCStr(geo_src), GL_GEOMETRY_SHADER);
        glAttachShader(shader->program, shader->geometry);
    }

    glLinkProgram(shader->program);

    int success;
    glGetProgramiv(shader->program, GL_LINK_STATUS, &success);
    if (success != GL_TRUE) {
        char log[1024];
        glGetProgramInfoLog(shader->program, 1024, NULL, log);
        rb_raise(rb_eRPGError, "failed to link shader program -- %s", log);
    }
    return Qnil;
}

GLuint rpg_shader_compile(const char *source, GLenum type) {

    int length = (int) strlen(source);
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, &length);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success == GL_TRUE)
        return shader;

    char log[512];
    glGetShaderInfoLog(shader, 512, NULL, log);
    printf(log);
    rb_raise(rb_eRPGError, "failed to compile shader -- %s", log);
    return 0;
}