#include "./shader.h"

VALUE rb_cShader;

void rpg_shader_init(VALUE parent) {
    rb_cShader = rb_define_class_under(parent, "Shader", rb_cObject);
    rb_define_alloc_func(rb_cShader, rpg_shader_alloc);
    rb_define_singleton_method(rb_cShader, "from_file", rpg_shader_from_file, -1);
    rb_define_method(rb_cShader, "initialize", rpg_shader_initialize, -1);
    rb_define_method(rb_cShader, "use", rpg_shader_use, 0);
    rb_define_method(rb_cShader, "current?", rpg_shader_current_p, 0);
    rb_define_method(rb_cShader, "locate", rpg_shader_locate, 1);
    rb_define_method(rb_cShader, "dispose", rpg_shader_dispose, 0);
    rb_define_method(rb_cShader, "disposed?", rpg_shader_disposed_p, 0);

    rb_define_method(rb_cShader, "program", rpg_shader_program, 0);
    rb_define_method(rb_cShader, "vertex", rpg_shader_vertex, 0);
    rb_define_method(rb_cShader, "fragment", rpg_shader_fragment, 0);
    rb_define_method(rb_cShader, "geometry", rpg_shader_geometry, 0);

    rb_define_method(rb_cShader, "uniformf", rpg_shader_uniformf, -1);
    rb_define_method(rb_cShader, "uniformi", rpg_shader_uniformi, -1);
    rb_define_method(rb_cShader, "uniform_color", rpg_shader_uniform_color, 2);
    rb_define_method(rb_cShader, "uniform_tone", rpg_shader_uniform_tone, 2);
    rb_define_method(rb_cShader, "uniform_vec2", rpg_shader_uniform_vec2, 2);
    rb_define_method(rb_cShader, "uniform_vec3", rpg_shader_uniform_vec3, 2);
    rb_define_method(rb_cShader, "uniform_vec4", rpg_shader_uniform_vec4, 2);
    rb_define_method(rb_cShader, "uniform_point", rpg_shader_uniform_point, 2);
    rb_define_method(rb_cShader, "uniform_size", rpg_shader_uniform_size, 2);
    rb_define_method(rb_cShader, "uniform_rect", rpg_shader_uniform_rect, 2);
    rb_define_method(rb_cShader, "uniform_mat4", rpg_shader_uniform_mat4, -1);
}

ALLOC_FUNC(rpg_shader_alloc, RPGshader)

ATTR_READER(rpg_shader_program, RPGshader, program, UINT2NUM)
ATTR_READER(rpg_shader_vertex, RPGshader, vertex, UINT2NUM)
ATTR_READER(rpg_shader_fragment, RPGshader, fragment, UINT2NUM)
ATTR_READER(rpg_shader_geometry, RPGshader, geometry, UINT2NUM)

static VALUE rpg_shader_use(VALUE self) {
    RPGshader *shader = DATA_PTR(self);
    if (rb_block_given_p()) {
        GLint id;
        glGetIntegerv(GL_CURRENT_PROGRAM, &id);
        glUseProgram(shader->program);
        rb_yield(self);
        glUseProgram(id);
        return Qnil;
    } else {
        glUseProgram(shader->program);
        return self;
    }
}

static VALUE rpg_shader_current_p(VALUE self) {
    RPGshader *shader = DATA_PTR(self);
    GLint id;
    glGetIntegerv(GL_CURRENT_PROGRAM, &id);
    return RB_BOOL(id == shader->program && id != 0);
}

static VALUE rpg_shader_dispose(VALUE self) {
    RPGshader *shader = DATA_PTR(self);
    if (shader->geometry) {
        glDeleteShader(shader->geometry);
    }
    if (shader->fragment) {
        glDeleteShader(shader->fragment);
    }
    if (shader->vertex) {
        glDeleteShader(shader->vertex);
    }
    if (shader->program) {
        glDeleteProgram(shader->program);
        shader->program = 0;
    }
    return Qnil;
}

static VALUE rpg_shader_disposed_p(VALUE self) {
    RPGshader *shader = DATA_PTR(self);
    return RB_BOOL(shader->program == 0);
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
    return INT2NUM(loc);
}

static VALUE rpg_shader_uniformf(int argc, VALUE *argv, VALUE self) {
    RPGshader *s = DATA_PTR(self);
    if (argc < 2) {
        rb_raise(rb_eArgError, "2 or more arguments required (given %d)", argc);
    }
    int loc = NUM2INT(argv[0]);
    switch (argc) {
        case 2: {
            if (RB_TYPE_P(argv[1], T_ARRAY)) {
                VALUE ary = argv[1];
                long len = rb_array_len(ary);
                float args[len];
                for (int i = 0; i < len; i++) {
                    args[i] = NUM2FLT(rb_ary_entry(ary, i));
                }
                glUniform1fv(loc, len, args);
            } else {
                glUniform1f(loc, NUM2FLT(argv[1]));
            }
            break;
        }
        case 3: {
            glUniform2f(loc, NUM2FLT(argv[1]), NUM2FLT(argv[2]));
            break;
        }
        case 4: {
            glUniform3f(loc, NUM2FLT(argv[1]), NUM2FLT(argv[2]), NUM2FLT(argv[3]));
            break;
        }
        case 5: {
            glUniform4f(loc, NUM2FLT(argv[1]), NUM2FLT(argv[2]), NUM2FLT(argv[3]), NUM2FLT(argv[4]));
            break;
        }
        default: {
            float args[argc - 1];
            for (int i = 1; i < argc; i++) {
                args[i - 1] = NUM2FLT(argv[i]);
            }
            glUniform1fv(loc, argc - 1, args);
        }
    }
    return self;    
}

static VALUE rpg_shader_uniformi(int argc, VALUE *argv, VALUE self) {
    RPGshader *s = DATA_PTR(self);
    if (argc < 2) {
        rb_raise(rb_eArgError, "2 or more arguments required (given %d)", argc);
    }
    int loc = NUM2INT(argv[0]);
    switch (argc) {
        case 2: {
            if (RB_TYPE_P(argv[1], T_ARRAY)) {
                VALUE ary = argv[1];
                long len = rb_array_len(ary);
                int args[len];
                for (int i = 0; i < len; i++) {
                    args[i] = NUM2INT(rb_ary_entry(ary, i));
                }
                glUniform1iv(loc, len, args);
            } else {
                glUniform1i(loc, NUM2INT(argv[1]));
            }
            break;
        }
        case 3: {
            glUniform2i(loc, NUM2INT(argv[1]), NUM2INT(argv[2]));
            break;
        }
        case 4: {
            glUniform3i(loc, NUM2INT(argv[1]), NUM2INT(argv[2]), NUM2INT(argv[3]));
            break;
        }
        case 5: {
            glUniform4i(loc, NUM2INT(argv[1]), NUM2INT(argv[2]), NUM2INT(argv[3]), NUM2INT(argv[4]));
            break;
        }
        default: {
            int args[argc - 1];
            for (int i = 1; i < argc; i++) {
                args[i - 1] = NUM2INT(argv[i]);
            }
            glUniform1iv(loc, argc - 1, args);
        }
    }
    return self;    
}

static VALUE rpg_shader_uniform_color(VALUE self, VALUE location, VALUE value) {
    RPGshader *s = DATA_PTR(self);
    RPGcolor *c = DATA_PTR(value);
    glUniform4f(NUM2INT(location), c->r, c->g, c->b, c->a);
    return self;    
}

static VALUE rpg_shader_uniform_tone(VALUE self, VALUE location, VALUE value) {
    RPGshader *s = DATA_PTR(self);
    RPGtone *t = DATA_PTR(value);
    glUniform4f(NUM2INT(location), t->r, t->g, t->b, t->gr);
    return self;    
}

static VALUE rpg_shader_uniform_vec2(VALUE self, VALUE location, VALUE value) {
    RPGshader *s = DATA_PTR(self);
    GLfloat *f = DATA_PTR(value);
    glUniform2f(NUM2INT(location), f[0], f[1]);
    return self;     
}

static VALUE rpg_shader_uniform_vec3(VALUE self, VALUE location, VALUE value) {
    RPGshader *s = DATA_PTR(self);
    GLfloat *f = DATA_PTR(value);
    glUniform3f(NUM2INT(location), f[0], f[1], f[2]);
    return self;      
}

static VALUE rpg_shader_uniform_vec4(VALUE self, VALUE location, VALUE value) {
    RPGshader *s = DATA_PTR(self);
    GLfloat *f = DATA_PTR(value);
    glUniform4f(NUM2INT(location), f[0], f[1], f[2], f[3]);
    return self;    
}

static VALUE rpg_shader_uniform_point(VALUE self, VALUE location, VALUE value) {
    RPGshader *s = DATA_PTR(self);
    GLint *i = DATA_PTR(value);
    glUniform2i(NUM2INT(location), i[0], i[1]);
    return self;      
}

static VALUE rpg_shader_uniform_size(VALUE self, VALUE location, VALUE value) {
    RPGshader *s = DATA_PTR(self);
    GLint *i = DATA_PTR(value);
    glUniform2i(NUM2INT(location), i[0], i[1]);
    return self;   
}

static VALUE rpg_shader_uniform_rect(VALUE self, VALUE location, VALUE value) {
    RPGshader *s = DATA_PTR(self);
    GLint *i = DATA_PTR(value);
    glUniform4i(NUM2INT(location), i[0], i[1], i[2], i[3]);
    return self;  
}

static VALUE rpg_shader_uniform_mat4(int argc, VALUE *argv, VALUE self) {
    VALUE location, matrix, transpose;
    rb_scan_args(argc, argv, "21", &location, &matrix, &transpose); 
    RPGshader *s = DATA_PTR(self);
    glUniformMatrix4fv(NUM2INT(location), 1, RTEST(transpose), DATA_PTR(matrix)); 
    return self;    
}