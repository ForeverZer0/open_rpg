#include "./shader.h"

VALUE rb_cShader;

void rpg_shader_init(VALUE parent) {
    rb_cShader = rb_define_class_under(parent, "Shader", rb_cObject);
    rb_define_alloc_func(rb_cShader, rpg_shader_alloc);
    rb_define_singleton_method(rb_cShader, "from_file", rpg_shader_from_file, -1);
    rb_define_method(rb_cShader, "initialize", rpg_shader_initialize, -1);
    rb_define_method(rb_cShader, "locate", rpg_shader_locate, 1);
}


static VALUE rpg_shader_alloc(VALUE klass) {
    RPGshader *shader = ALLOC(RPGshader);
    memset(shader, 0, sizeof(RPGshader));
    return Data_Wrap_Struct(klass, NULL, rpg_shader_free, shader);
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
    return Data_Wrap_Struct(klass, NULL, rpg_shader_free, shader);
}

static VALUE rpg_shader_locate(VALUE self, VALUE value) {
    RPGshader *shader = DATA_PTR(self);
    GLint loc = glGetUniformLocation(shader->program, StringValueCStr(value));
    // TODO: Cache location?
    return INT2NUM(loc);
}