#include "./shader.h"

VALUE rb_cShader;

void rpg_shader_init(VALUE parent) {
    rb_cShader = rb_define_class_under(parent, "Shader", rb_cObject);
    rb_define_alloc_func(rb_cShader, rpg_shader_alloc);
    rb_define_singleton_method(rb_cShader, "from_file", rpg_shader_from_file, -1);
    rb_define_method(rb_cShader, "initialize", rpg_shader_initialize, -1);
    rb_define_method(rb_cShader, "dispose", rpg_shader_dispose, 0);
    rb_define_method(rb_cShader, "disposed?", rpg_shader_disposed_p, 0);

    rb_define_method(rb_cShader, "locate", rpg_shader_locate, 1);

    rb_include_module(rb_cShader, rb_mDisposable);
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

static inline void rpg_shader_link(RPGshader *shader) {
    shader->program = glCreateProgram();
    glAttachShader(shader->program, shader->vertex);
    glAttachShader(shader->program, shader->fragment);
    if (shader->geometry) {
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
}

static VALUE rpg_shader_initialize(int argc, VALUE *argv, VALUE self) {
    VALUE vert_src, frag_src, geo_src;
    rb_scan_args(argc, argv, "21", &vert_src, &frag_src, &geo_src);

    const char *vsrc = StringValueCStr(vert_src);
    const char *fsrc = StringValueCStr(frag_src);
    const char *gsrc = NIL_P(geo_src) ? NULL : StringValueCStr(geo_src);

    RPGshader *shader = DATA_PTR(self);
    shader->vertex = rpg_create_shader_src(vsrc, GL_VERTEX_SHADER);
    shader->fragment = rpg_create_shader_src(fsrc, GL_FRAGMENT_SHADER);
    shader->geometry = gsrc ? rpg_create_shader_src(gsrc, GL_GEOMETRY_SHADER) : 0;
    rpg_shader_link(shader);
    return Qnil;
}

static VALUE rpg_shader_from_file(int argc, VALUE *argv, VALUE klass) {
    VALUE vert_src, frag_src, geo_src;
    rb_scan_args(argc, argv, "21", &vert_src, &frag_src, &geo_src);

    const char *vsrc = StringValueCStr(vert_src);
    const char *fsrc = StringValueCStr(frag_src);
    const char *gsrc = NIL_P(geo_src) ? NULL : StringValueCStr(geo_src);

    RPGshader *shader = ALLOC(RPGshader);
    shader->vertex = rpg_create_shader(vsrc, GL_VERTEX_SHADER);
    shader->fragment = rpg_create_shader(fsrc, GL_FRAGMENT_SHADER);
    shader->geometry = gsrc ? rpg_create_shader(gsrc, GL_GEOMETRY_SHADER) : 0;
    rpg_shader_link(shader);
    return Data_Wrap_Struct(klass, NULL, RUBY_DEFAULT_FREE, shader);
}

static VALUE rpg_shader_locate(VALUE self, VALUE value) {
    RPGshader *shader = DATA_PTR(self);
    GLint loc = glGetUniformLocation(shader->program, StringValueCStr(value));
    // TODO: Cache location?
    return INT2NUM(loc);
}