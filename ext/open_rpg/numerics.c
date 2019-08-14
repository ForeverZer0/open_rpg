#include "./internal.h"
#include "./rpg.h"

VALUE rb_cVec2;
VALUE rb_cVec3;
VALUE rb_cVec4;
VALUE rb_cMat3;
VALUE rb_cMat4;

#define VEC2_SET(_vec, _x, _y)                                                                                                             \
    _vec->x = _x;                                                                                                                          \
    _vec->y = _y

#define VEC3_SET(_vec, _x, _y, _z)                                                                                                         \
    _vec->x = _x;                                                                                                                          \
    _vec->y = _y;                                                                                                                          \
    _vec->z = _z

#define VEC4_SET(_vec, _x, _y, _z, _w)                                                                                                     \
    _vec->x = _x;                                                                                                                          \
    _vec->y = _y;                                                                                                                          \
    _vec->z = _z;                                                                                                                          \
    _vec->w = _w

#define MAT3_SET(_mat, _m11, _m12, _m3, _m21, _m22, _m23, _m31, _m32, _m33)                                                                \
    _mat->m11 = _m11;                                                                                                                      \
    _mat->m12 = _m12;                                                                                                                      \
    _mat->m13 = _m13;                                                                                                                      \
    _mat->m21 = _m21;                                                                                                                      \
    _mat->m22 = _m22;                                                                                                                      \
    _mat->m23 = _m23;                                                                                                                      \
    _mat->m31 = _m31;                                                                                                                      \
    _mat->m32 = _m32;                                                                                                                      \
    _mat->m33 = _m33

#define EACH_FUNC(function, type)                                                                                                          \
    static VALUE function(VALUE self) {                                                                                                    \
        long int len = sizeof(type) / sizeof(GLfloat);                                                                                     \
        volatile VALUE vec = self;                                                                                                         \
        RETURN_SIZED_ENUMERATOR(vec, 0, 0, len);                                                                                           \
        GLfloat *obj = DATA_PTR(self);                                                                                                     \
        for (int i = 0; i < len; i++) {                                                                                                    \
            rb_yield(DBL2NUM(obj[i]));                                                                                                     \
        }                                                                                                                                  \
        return self;                                                                                                                       \
    }

#define EQUAL_FUNC(function, type)                                                                                                         \
    static VALUE function(VALUE self, VALUE other) {                                                                                       \
        if (CLASS_OF(self) != CLASS_OF(other)) {                                                                                           \
            return Qfalse;                                                                                                                 \
        }                                                                                                                                  \
        GLfloat *v1 = DATA_PTR(self);                                                                                                      \
        GLfloat *v2 = DATA_PTR(other);                                                                                                     \
        int len = sizeof(type) / sizeof(GLfloat);                                                                                          \
        for (int i = 0; i < len; i++) {                                                                                                    \
            if (!FLT_EQL(v1[i], v2[i])) {                                                                                                  \
                return Qfalse;                                                                                                             \
            }                                                                                                                              \
        }                                                                                                                                  \
        return Qtrue;                                                                                                                      \
    }

ALLOC_FUNC(rpg_vec2_alloc, RPGvec2)
ALLOC_FUNC(rpg_vec3_alloc, RPGvec3)
ALLOC_FUNC(rpg_vec4_alloc, RPGvec4)
ALLOC_FUNC(rpg_mat3_alloc, RPGmat3)
ALLOC_FUNC(rpg_mat4_alloc, RPGmat4)

EACH_FUNC(rpg_vec2_each, RPGvec2)
EACH_FUNC(rpg_vec3_each, RPGvec3)
EACH_FUNC(rpg_vec4_each, RPGvec4)
EACH_FUNC(rpg_mat3_each, RPGmat3)
EACH_FUNC(rpg_mat4_each, RPGmat4)

EQUAL_FUNC(rpg_vec2_equal, RPGvec2)
EQUAL_FUNC(rpg_vec3_equal, RPGvec3)
EQUAL_FUNC(rpg_vec4_equal, RPGvec4)
EQUAL_FUNC(rpg_mat3_equal, RPGmat3)
EQUAL_FUNC(rpg_mat4_equal, RPGmat4)

DUP_FUNC(rpg_vec2_dup, RPGvec2)
DUP_FUNC(rpg_vec3_dup, RPGvec3)
DUP_FUNC(rpg_vec4_dup, RPGvec4)
DUP_FUNC(rpg_mat3_dup, RPGmat3)
DUP_FUNC(rpg_mat4_dup, RPGmat4)

DUMP_FUNC(rpg_vec2_dump, RPGvec2)
DUMP_FUNC(rpg_vec3_dump, RPGvec3)
DUMP_FUNC(rpg_vec4_dump, RPGvec4)
DUMP_FUNC(rpg_mat3_dump, RPGmat3)
DUMP_FUNC(rpg_mat4_dump, RPGmat4)

LOAD_FUNC(rpg_vec2_load, RPGvec2)
LOAD_FUNC(rpg_vec3_load, RPGvec3)
LOAD_FUNC(rpg_vec4_load, RPGvec4)
LOAD_FUNC(rpg_mat3_load, RPGmat3)
LOAD_FUNC(rpg_mat4_load, RPGmat4)

ATTR_READER(rpg_vec2_get_x, RPGvec2, x, DBL2NUM)
ATTR_READER(rpg_vec2_get_y, RPGvec2, y, DBL2NUM)
ATTR_WRITER(rpg_vec2_set_x, RPGvec2, x, NUM2FLT)
ATTR_WRITER(rpg_vec2_set_y, RPGvec2, y, NUM2FLT)

ATTR_READER(rpg_vec3_get_x, RPGvec3, x, DBL2NUM)
ATTR_READER(rpg_vec3_get_y, RPGvec3, y, DBL2NUM)
ATTR_READER(rpg_vec3_get_z, RPGvec3, z, DBL2NUM)
ATTR_WRITER(rpg_vec3_set_x, RPGvec3, x, NUM2FLT)
ATTR_WRITER(rpg_vec3_set_y, RPGvec3, y, NUM2FLT)
ATTR_WRITER(rpg_vec3_set_z, RPGvec3, z, NUM2FLT)

ATTR_READER(rpg_vec4_get_x, RPGvec4, x, DBL2NUM)
ATTR_READER(rpg_vec4_get_y, RPGvec4, y, DBL2NUM)
ATTR_READER(rpg_vec4_get_z, RPGvec4, z, DBL2NUM)
ATTR_READER(rpg_vec4_get_w, RPGvec4, w, DBL2NUM)
ATTR_WRITER(rpg_vec4_set_x, RPGvec4, x, NUM2FLT)
ATTR_WRITER(rpg_vec4_set_y, RPGvec4, y, NUM2FLT)
ATTR_WRITER(rpg_vec4_set_z, RPGvec4, z, NUM2FLT)
ATTR_WRITER(rpg_vec4_set_w, RPGvec4, w, NUM2FLT)

ATTR_READER(rpg_mat3_get_m11, RPGmat3, m11, DBL2NUM)
ATTR_READER(rpg_mat3_get_m12, RPGmat3, m12, DBL2NUM)
ATTR_READER(rpg_mat3_get_m13, RPGmat3, m13, DBL2NUM)
ATTR_READER(rpg_mat3_get_m21, RPGmat3, m21, DBL2NUM)
ATTR_READER(rpg_mat3_get_m22, RPGmat3, m22, DBL2NUM)
ATTR_READER(rpg_mat3_get_m23, RPGmat3, m23, DBL2NUM)
ATTR_READER(rpg_mat3_get_m31, RPGmat3, m31, DBL2NUM)
ATTR_READER(rpg_mat3_get_m32, RPGmat3, m32, DBL2NUM)
ATTR_READER(rpg_mat3_get_m33, RPGmat3, m33, DBL2NUM)
ATTR_WRITER(rpg_mat3_set_m11, RPGmat3, m11, NUM2FLT)
ATTR_WRITER(rpg_mat3_set_m12, RPGmat3, m12, NUM2FLT)
ATTR_WRITER(rpg_mat3_set_m13, RPGmat3, m13, NUM2FLT)
ATTR_WRITER(rpg_mat3_set_m21, RPGmat3, m21, NUM2FLT)
ATTR_WRITER(rpg_mat3_set_m22, RPGmat3, m22, NUM2FLT)
ATTR_WRITER(rpg_mat3_set_m23, RPGmat3, m23, NUM2FLT)
ATTR_WRITER(rpg_mat3_set_m31, RPGmat3, m31, NUM2FLT)
ATTR_WRITER(rpg_mat3_set_m32, RPGmat3, m32, NUM2FLT)
ATTR_WRITER(rpg_mat3_set_m33, RPGmat3, m33, NUM2FLT)

ATTR_READER(rpg_mat4_get_m11, RPGmat4, m11, DBL2NUM)
ATTR_READER(rpg_mat4_get_m12, RPGmat4, m12, DBL2NUM)
ATTR_READER(rpg_mat4_get_m13, RPGmat4, m13, DBL2NUM)
ATTR_READER(rpg_mat4_get_m14, RPGmat4, m14, DBL2NUM)
ATTR_READER(rpg_mat4_get_m21, RPGmat4, m21, DBL2NUM)
ATTR_READER(rpg_mat4_get_m22, RPGmat4, m22, DBL2NUM)
ATTR_READER(rpg_mat4_get_m23, RPGmat4, m23, DBL2NUM)
ATTR_READER(rpg_mat4_get_m24, RPGmat4, m24, DBL2NUM)
ATTR_READER(rpg_mat4_get_m31, RPGmat4, m31, DBL2NUM)
ATTR_READER(rpg_mat4_get_m32, RPGmat4, m32, DBL2NUM)
ATTR_READER(rpg_mat4_get_m33, RPGmat4, m33, DBL2NUM)
ATTR_READER(rpg_mat4_get_m34, RPGmat4, m34, DBL2NUM)
ATTR_READER(rpg_mat4_get_m41, RPGmat4, m41, DBL2NUM)
ATTR_READER(rpg_mat4_get_m42, RPGmat4, m42, DBL2NUM)
ATTR_READER(rpg_mat4_get_m43, RPGmat4, m43, DBL2NUM)
ATTR_READER(rpg_mat4_get_m44, RPGmat4, m44, DBL2NUM)
ATTR_WRITER(rpg_mat4_set_m11, RPGmat4, m11, NUM2FLT)
ATTR_WRITER(rpg_mat4_set_m12, RPGmat4, m12, NUM2FLT)
ATTR_WRITER(rpg_mat4_set_m13, RPGmat4, m13, NUM2FLT)
ATTR_WRITER(rpg_mat4_set_m14, RPGmat4, m14, NUM2FLT)
ATTR_WRITER(rpg_mat4_set_m21, RPGmat4, m21, NUM2FLT)
ATTR_WRITER(rpg_mat4_set_m22, RPGmat4, m22, NUM2FLT)
ATTR_WRITER(rpg_mat4_set_m23, RPGmat4, m23, NUM2FLT)
ATTR_WRITER(rpg_mat4_set_m24, RPGmat4, m24, NUM2FLT)
ATTR_WRITER(rpg_mat4_set_m31, RPGmat4, m31, NUM2FLT)
ATTR_WRITER(rpg_mat4_set_m32, RPGmat4, m32, NUM2FLT)
ATTR_WRITER(rpg_mat4_set_m33, RPGmat4, m33, NUM2FLT)
ATTR_WRITER(rpg_mat4_set_m34, RPGmat4, m34, NUM2FLT)
ATTR_WRITER(rpg_mat4_set_m41, RPGmat4, m41, NUM2FLT)
ATTR_WRITER(rpg_mat4_set_m42, RPGmat4, m42, NUM2FLT)
ATTR_WRITER(rpg_mat4_set_m43, RPGmat4, m43, NUM2FLT)
ATTR_WRITER(rpg_mat4_set_m44, RPGmat4, m44, NUM2FLT)

static VALUE rpg_vec2_initialize(int argc, VALUE *argv, VALUE self) {
    VALUE a1, a2;
    rb_scan_args(argc, argv, "02", &a1, &a2);
    RPGvec2 *v = DATA_PTR(self);
    switch (argc) {
        case 1: {
            if (RB_TYPE_P(a1, T_DATA)) {
                float *other = DATA_PTR(a1);
                memcpy(v, other, sizeof(RPGvec2));
            } else {
                float f = NUM2FLT(a1);
                VEC2_SET(v, f, f);
            }
            break;
        }
        case 2: {
            VEC2_SET(v, NUM2FLT(a1), NUM2FLT(a2));
            break;
        }
    }
    return Qnil;
}

static VALUE rpg_vec3_initialize(int argc, VALUE *argv, VALUE self) {
    VALUE a1, a2, a3;
    rb_scan_args(argc, argv, "03", &a1, &a2, &a3);
    RPGvec3 *v = DATA_PTR(self);
    switch (argc) {
        case 1: {
            if (RB_TYPE_P(a1, T_DATA)) {
                float *other = DATA_PTR(a1);
                memcpy(v, other, sizeof(RPGvec3));
            } else {
                float f = NUM2FLT(a1);
                VEC3_SET(v, f, f, f);
            }
            break;
        }
        case 2: {
            float *f2;
            if (RB_FLONUM_P(a1)) {
                f2 = DATA_PTR(a2);
                VEC3_SET(v, NUM2FLT(a1), f2[0], f2[1]);
            } else {
                f2 = DATA_PTR(a1);
                VEC3_SET(v, f2[0], f2[1], NUM2FLT(a2));
            }
            break;
        }
        case 3: {
            VEC3_SET(v, NUM2FLT(a1), NUM2FLT(a2), NUM2FLT(a3));
            break;
        }
    }
    return Qnil;
}

static VALUE rpg_vec4_initialize(int argc, VALUE *argv, VALUE self) {
    VALUE a1, a2, a3, a4;
    rb_scan_args(argc, argv, "04", &a1, &a2, &a3, &a4);
    RPGvec4 *v = DATA_PTR(self);

    switch (argc) {
        case 1: {
            if (RB_TYPE_P(a1, T_DATA)) {
                float *other = DATA_PTR(a1);
                memcpy(v, other, sizeof(RPGvec4));
            } else {
                float f = NUM2FLT(a1);
                VEC4_SET(v, f, f, f, f);
            }
            break;
        }
        case 2: {
            if (RB_IS_A(a1, rb_cVec2)) {
                RPGvec2 *v1 = DATA_PTR(a1);
                RPGvec2 *v2 = DATA_PTR(a2);
                VEC4_SET(v, v1->x, v1->y, v2->x, v2->y);
            } else if (RB_IS_A(a1, rb_cVec3)) {
                RPGvec3 *vec3 = DATA_PTR(a1);
                VEC4_SET(v, vec3->x, vec3->y, vec3->z, NUM2FLT(a2));
            } else {
                RPGvec3 *vec3 = DATA_PTR(a2);
                VEC4_SET(v, NUM2FLT(a1), vec3->x, vec3->y, vec3->z);
            }
            break;
        }
        case 3: {
            RPGvec2 *vec2;
            if (RB_IS_A(a1, rb_cVec2)) {
                vec2 = DATA_PTR(a1);
                VEC4_SET(v, vec2->x, vec2->y, NUM2FLT(a2), NUM2FLT(a3));
            } else if (RB_IS_A(a2, rb_cVec2)) {
                vec2 = DATA_PTR(a2);
                VEC4_SET(v, NUM2FLT(a1), vec2->x, vec2->y, NUM2FLT(a3));
            } else {
                vec2 = DATA_PTR(a3);
                VEC4_SET(v, NUM2FLT(a1), NUM2FLT(a2), vec2->x, vec2->y);
            }
            break;
        }
        case 4: {
            VEC4_SET(v, NUM2FLT(a1), NUM2FLT(a2), NUM2FLT(a3), NUM2FLT(a4));
            break;
        }
    }
    return Qnil;
}

static VALUE rpg_mat3_initialize(int argc, VALUE *argv, VALUE self) {
    if (argc != 0 && argc != 6) {
        rb_raise(rb_eArgError, "wrong number of arguments (given %d, expected 0, 6)", argc);
    }
    if (argc == 6) {
        float *mat = DATA_PTR(self);
        for (int i = 0; i < 6; i++) {
            mat[i] = NUM2FLT(argv[i]); // TODO:
        }
    }
    return Qnil;
}

static VALUE rpg_mat4_initialize(int argc, VALUE *argv, VALUE self) {
    switch (argc) {
        case 0:
            break;
        case 1: {
            if (RB_IS_A(argv[0], rb_cMat3)) {
                rb_raise(rb_eTypeError, "expected Matrix3x3");
            }
            RPGmat4 *mat4 = DATA_PTR(self);
            RPGmat3 *mat3 = DATA_PTR(argv[0]);

            mat4->m11 = mat3->m11;
            mat4->m12 = mat3->m12;
            mat4->m13 = mat3->m13;

            mat4->m21 = mat3->m21;
            mat4->m22 = mat3->m22;
            mat4->m23 = mat3->m23;

            mat4->m31 = mat3->m31;
            mat4->m32 = mat3->m32;
            mat4->m33 = mat3->m33;

            mat4->m41 = mat4->m42 = mat4->m43 = 0.0f;
            mat4->m44 = 1.0f;
            break;
        }
        case 16: {
            float *mat = DATA_PTR(self);
            for (int i = 0; i < 16; i++) {
                mat[i] = NUM2FLT(argv[i]);
            }
            break;
        }
        default: {
            rb_raise(rb_eArgError, "wrong number of arguments (given %d, expected 0, 1, 16)", argc);
            break;
        }
    }
    return Qnil;
}

static VALUE rpg_vec2_inspect(VALUE self) {
    RPGvec4 *v = DATA_PTR(self);
    return rb_sprintf("<Vector2: x:%f y:%f>", v->x, v->y);
}

static VALUE rpg_vec3_inspect(VALUE self) {
    RPGvec3 *v = DATA_PTR(self);
    return rb_sprintf("<Vector3: x:%f y:%f z:%f>", v->x, v->y, v->z);
}

static VALUE rpg_vec4_inspect(VALUE self) {
    RPGvec4 *v = DATA_PTR(self);
    return rb_sprintf("<Vector4: x:%f y:%f z:%f w:%f>", v->x, v->y, v->z, v->w);
}

static VALUE rpg_mat3_inspect(VALUE self) {
    RPGmat3 *m = DATA_PTR(self);
    return rb_sprintf("<Matrix3x3: %f, %f, %f, %f, %f, %f, %f, %f, %f>", m->m11, m->m12, m->m13, m->m21, m->m22, m->m23, m->m31, m->m32,
                      m->m33);
}

static VALUE rpg_mat4_inspect(VALUE self) {
    RPGmat4 *m = DATA_PTR(self);
    return rb_sprintf("<Matrix4x4: %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f>", m->m11, m->m12, m->m13,
                      m->m14, m->m21, m->m22, m->m23, m->m24, m->m31, m->m32, m->m33, m->m34, m->m41, m->m42, m->m43, m->m44);
}

void rpg_numerics_init(VALUE parent) {
    VALUE num = rb_define_module_under(parent, "Numerics");
    rb_cVec2 = rb_define_class_under(num, "Vector2", rb_cObject);
    rb_cVec3 = rb_define_class_under(num, "Vector3", rb_cObject);
    rb_cVec4 = rb_define_class_under(num, "Vector4", rb_cObject);
    rb_cMat3 = rb_define_class_under(num, "Matrix3x3", rb_cObject);
    rb_cMat4 = rb_define_class_under(num, "Matrix4x4", rb_cObject);

    rb_include_module(rb_cVec2, rb_mEnumerable);
    rb_include_module(rb_cVec3, rb_mEnumerable);
    rb_include_module(rb_cVec4, rb_mEnumerable);
    rb_include_module(rb_cMat3, rb_mEnumerable);
    rb_include_module(rb_cMat4, rb_mEnumerable);

    rb_define_alloc_func(rb_cVec2, rpg_vec2_alloc);
    rb_define_alloc_func(rb_cVec3, rpg_vec3_alloc);
    rb_define_alloc_func(rb_cVec4, rpg_vec4_alloc);
    rb_define_alloc_func(rb_cMat3, rpg_mat3_alloc);
    rb_define_alloc_func(rb_cMat4, rpg_mat4_alloc);

    rb_define_method(rb_cVec2, "initialize", rpg_vec2_initialize, -1);
    rb_define_method(rb_cVec3, "initialize", rpg_vec3_initialize, -1);
    rb_define_method(rb_cVec4, "initialize", rpg_vec4_initialize, -1);
    rb_define_method(rb_cMat3, "initialize", rpg_mat3_initialize, -1);
    rb_define_method(rb_cMat4, "initialize", rpg_mat4_initialize, -1);

    rb_define_singleton_method(rb_cVec2, "zero", rpg_vec2_alloc, 0);
    rb_define_singleton_method(rb_cVec3, "zero", rpg_vec3_alloc, 0);
    rb_define_singleton_method(rb_cVec4, "zero", rpg_vec4_alloc, 0);
    rb_define_singleton_method(rb_cMat3, "zero", rpg_mat3_alloc, 0);
    rb_define_singleton_method(rb_cMat4, "zero", rpg_mat4_alloc, 0);

    rb_define_method(rb_cVec2, "each", rpg_vec2_each, 0);
    rb_define_method(rb_cVec3, "each", rpg_vec3_each, 0);
    rb_define_method(rb_cVec4, "each", rpg_vec4_each, 0);
    rb_define_method(rb_cMat3, "each", rpg_mat3_each, 0);
    rb_define_method(rb_cMat4, "each", rpg_mat4_each, 0);

    rb_define_method(rb_cVec2, "==", rpg_vec2_equal, 1);
    rb_define_method(rb_cVec3, "==", rpg_vec3_equal, 1);
    rb_define_method(rb_cVec4, "==", rpg_vec4_equal, 1);
    rb_define_method(rb_cMat3, "==", rpg_mat3_equal, 1);
    rb_define_method(rb_cMat4, "==", rpg_mat4_equal, 1);

    rb_define_method(rb_cVec2, "inspect", rpg_vec2_inspect, 0);
    rb_define_method(rb_cVec3, "inspect", rpg_vec3_inspect, 0);
    rb_define_method(rb_cVec4, "inspect", rpg_vec4_inspect, 0);
    rb_define_method(rb_cMat3, "inspect", rpg_mat3_inspect, 0);
    rb_define_method(rb_cMat4, "inspect", rpg_mat4_inspect, 0);

    rb_define_singleton_method(rb_cVec2, "_load", rpg_vec2_load, 1);
    rb_define_singleton_method(rb_cVec3, "_load", rpg_vec3_load, 1);
    rb_define_singleton_method(rb_cVec4, "_load", rpg_vec4_load, 1);
    rb_define_singleton_method(rb_cMat3, "_load", rpg_mat3_load, 1);
    rb_define_singleton_method(rb_cMat4, "_load", rpg_mat4_load, 1);

    rb_define_method(rb_cVec2, "_dump", rpg_vec2_dump, -1);
    rb_define_method(rb_cVec3, "_dump", rpg_vec3_dump, -1);
    rb_define_method(rb_cVec4, "_dump", rpg_vec4_dump, -1);
    rb_define_method(rb_cMat3, "_dump", rpg_mat3_dump, -1);
    rb_define_method(rb_cMat4, "_dump", rpg_mat4_dump, -1);

    rb_define_method(rb_cVec2, "x", rpg_vec2_get_x, 0);
    rb_define_method(rb_cVec2, "y", rpg_vec2_get_y, 0);
    rb_define_method(rb_cVec2, "x=", rpg_vec2_set_x, 1);
    rb_define_method(rb_cVec2, "y=", rpg_vec2_set_y, 1);

    rb_define_method(rb_cVec3, "x", rpg_vec3_get_x, 0);
    rb_define_method(rb_cVec3, "y", rpg_vec3_get_y, 0);
    rb_define_method(rb_cVec3, "z", rpg_vec3_get_z, 0);
    rb_define_method(rb_cVec3, "x=", rpg_vec3_set_x, 1);
    rb_define_method(rb_cVec3, "y=", rpg_vec3_set_y, 1);
    rb_define_method(rb_cVec3, "z=", rpg_vec3_set_z, 1);

    rb_define_method(rb_cVec4, "x", rpg_vec4_get_x, 0);
    rb_define_method(rb_cVec4, "y", rpg_vec4_get_y, 0);
    rb_define_method(rb_cVec4, "z", rpg_vec4_get_z, 0);
    rb_define_method(rb_cVec4, "w", rpg_vec4_get_w, 0);
    rb_define_method(rb_cVec4, "x=", rpg_vec4_set_x, 1);
    rb_define_method(rb_cVec4, "y=", rpg_vec4_set_y, 1);
    rb_define_method(rb_cVec4, "z=", rpg_vec4_set_z, 1);
    rb_define_method(rb_cVec4, "w=", rpg_vec4_set_w, 1);

    rb_define_method(rb_cMat3, "m11", rpg_mat3_get_m11, 0);
    rb_define_method(rb_cMat3, "m12", rpg_mat3_get_m12, 0);
    rb_define_method(rb_cMat3, "m13", rpg_mat3_get_m13, 0);
    rb_define_method(rb_cMat3, "m21", rpg_mat3_get_m21, 0);
    rb_define_method(rb_cMat3, "m22", rpg_mat3_get_m22, 0);
    rb_define_method(rb_cMat3, "m23", rpg_mat3_get_m23, 0);
    rb_define_method(rb_cMat3, "m31", rpg_mat3_get_m31, 0);
    rb_define_method(rb_cMat3, "m32", rpg_mat3_get_m32, 0);
    rb_define_method(rb_cMat3, "m33", rpg_mat3_get_m33, 0);
    rb_define_method(rb_cMat3, "m11=", rpg_mat3_set_m11, 1);
    rb_define_method(rb_cMat3, "m12=", rpg_mat3_set_m12, 1);
    rb_define_method(rb_cMat3, "m13=", rpg_mat3_set_m13, 1);
    rb_define_method(rb_cMat3, "m21=", rpg_mat3_set_m21, 1);
    rb_define_method(rb_cMat3, "m22=", rpg_mat3_set_m22, 1);
    rb_define_method(rb_cMat3, "m23=", rpg_mat3_set_m23, 1);
    rb_define_method(rb_cMat3, "m31=", rpg_mat3_set_m31, 1);
    rb_define_method(rb_cMat3, "m32=", rpg_mat3_set_m32, 1);
    rb_define_method(rb_cMat3, "m33=", rpg_mat3_set_m33, 1);

    rb_define_method(rb_cMat4, "m11", rpg_mat4_get_m11, 0);
    rb_define_method(rb_cMat4, "m12", rpg_mat4_get_m12, 0);
    rb_define_method(rb_cMat4, "m13", rpg_mat4_get_m13, 0);
    rb_define_method(rb_cMat4, "m14", rpg_mat4_get_m14, 0);
    rb_define_method(rb_cMat4, "m21", rpg_mat4_get_m21, 0);
    rb_define_method(rb_cMat4, "m22", rpg_mat4_get_m22, 0);
    rb_define_method(rb_cMat4, "m23", rpg_mat4_get_m23, 0);
    rb_define_method(rb_cMat4, "m24", rpg_mat4_get_m24, 0);
    rb_define_method(rb_cMat4, "m31", rpg_mat4_get_m31, 0);
    rb_define_method(rb_cMat4, "m32", rpg_mat4_get_m32, 0);
    rb_define_method(rb_cMat4, "m33", rpg_mat4_get_m33, 0);
    rb_define_method(rb_cMat4, "m34", rpg_mat4_get_m34, 0);
    rb_define_method(rb_cMat4, "m41", rpg_mat4_get_m41, 0);
    rb_define_method(rb_cMat4, "m42", rpg_mat4_get_m42, 0);
    rb_define_method(rb_cMat4, "m43", rpg_mat4_get_m43, 0);
    rb_define_method(rb_cMat4, "m44", rpg_mat4_get_m44, 0);
    rb_define_method(rb_cMat4, "m11=", rpg_mat4_set_m11, 1);
    rb_define_method(rb_cMat4, "m12=", rpg_mat4_set_m12, 1);
    rb_define_method(rb_cMat4, "m13=", rpg_mat4_set_m13, 1);
    rb_define_method(rb_cMat4, "m14=", rpg_mat4_set_m14, 1);
    rb_define_method(rb_cMat4, "m21=", rpg_mat4_set_m21, 1);
    rb_define_method(rb_cMat4, "m22=", rpg_mat4_set_m22, 1);
    rb_define_method(rb_cMat4, "m23=", rpg_mat4_set_m23, 1);
    rb_define_method(rb_cMat4, "m24=", rpg_mat4_set_m24, 1);
    rb_define_method(rb_cMat4, "m31=", rpg_mat4_set_m31, 1);
    rb_define_method(rb_cMat4, "m32=", rpg_mat4_set_m32, 1);
    rb_define_method(rb_cMat4, "m33=", rpg_mat4_set_m33, 1);
    rb_define_method(rb_cMat4, "m34=", rpg_mat4_set_m34, 1);
    rb_define_method(rb_cMat4, "m41=", rpg_mat4_set_m41, 1);
    rb_define_method(rb_cMat4, "m42=", rpg_mat4_set_m42, 1);
    rb_define_method(rb_cMat4, "m43=", rpg_mat4_set_m43, 1);
    rb_define_method(rb_cMat4, "m44=", rpg_mat4_set_m44, 1);
}
