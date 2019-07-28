#include "numerics.h"

VALUE rb_cVector2;
VALUE rb_cVector3;
VALUE rb_cVector4;
VALUE rb_cQuaternion;
VALUE rb_cMatrix3x2;
VALUE rb_cMatrix4x4;

#define EACH_FUNC(function, type)                   \
static VALUE function(VALUE self) {                 \
    long int len = sizeof(type) / sizeof(GLfloat);  \
    volatile VALUE vec = self;                      \
    RETURN_SIZED_ENUMERATOR(vec, 0, 0, len);        \
    GLfloat *obj = DATA_PTR(self);                  \
    for (int i = 0; i < len; i++) {                 \
        rb_yield(DBL2NUM(obj[i]));                  \
    }                                               \
    return self;                                    \
}

#define EQUAL_FUNC(function, type)                  \
static VALUE function(VALUE self, VALUE other) {    \
   if (CLASS_OF(self) != CLASS_OF(other)) {         \
       return Qfalse;                               \
   }                                                \
   GLfloat *v1 = DATA_PTR(self);                    \
   GLfloat *v2 = DATA_PTR(other);                   \
   int len = sizeof(type) / sizeof(GLfloat);        \
   for (int i = 0; i < len; i++) {                  \
       if (!FLT_EQL(v1[i], v2[i])) {                \
           return Qfalse;                           \
       }                                            \
   }                                                \
   return Qtrue;                                    \
}

void rpg_numerics_init(VALUE parent) {
    rb_cVector2 = rb_define_class_under(parent, "Vector2", rb_cObject);
    rb_cVector3 = rb_define_class_under(parent, "Vector3", rb_cObject);
    rb_cVector4 = rb_define_class_under(parent, "Vector4", rb_cObject);
    rb_cQuaternion = rb_define_class_under(parent, "Quaternion", rb_cObject);
    rb_cMatrix3x2 = rb_define_class_under(parent, "Matrix3x2", rb_cObject);
    rb_cMatrix4x4 = rb_define_class_under(parent, "Matrix4x4", rb_cObject);

    rb_include_module(rb_cVector2, rb_mEnumerable);
    rb_include_module(rb_cVector3, rb_mEnumerable);
    rb_include_module(rb_cVector4, rb_mEnumerable);
    rb_include_module(rb_cQuaternion, rb_mEnumerable);
    rb_include_module(rb_cMatrix3x2, rb_mEnumerable);
    rb_include_module(rb_cMatrix4x4, rb_mEnumerable);

    rb_define_alloc_func(rb_cVector2, rpg_vec2_alloc);
    rb_define_alloc_func(rb_cVector3, rpg_vec3_alloc);
    rb_define_alloc_func(rb_cVector4, rpg_vec4_alloc);
    rb_define_alloc_func(rb_cQuaternion, rpg_quat_alloc);
    rb_define_alloc_func(rb_cMatrix3x2, rpg_mat3_alloc);
    rb_define_alloc_func(rb_cMatrix4x4, rpg_mat4_alloc);

    rb_define_method(rb_cVector2, "initialize", rpg_vec2_initialize, -1);
    rb_define_method(rb_cVector3, "initialize", rpg_vec3_initialize, -1);
    rb_define_method(rb_cVector4, "initialize", rpg_vec4_initialize, -1);
    rb_define_method(rb_cQuaternion, "initialize", rpg_vec4_initialize, -1);
    rb_define_method(rb_cMatrix3x2, "initialize", rpg_mat3_initialize, -1);
    rb_define_method(rb_cMatrix4x4, "initialize", rpg_mat4_initialize, -1);

    rb_define_singleton_method(rb_cVector2, "zero", rpg_vec2_alloc, 0);
    rb_define_singleton_method(rb_cVector3, "zero", rpg_vec3_alloc, 0);
    rb_define_singleton_method(rb_cVector4, "zero", rpg_vec4_alloc, 0);
    rb_define_singleton_method(rb_cQuaternion, "zero", rpg_quat_alloc, 0);
    rb_define_singleton_method(rb_cMatrix3x2, "zero", rpg_mat3_alloc, 0);
    rb_define_singleton_method(rb_cMatrix4x4, "zero", rpg_mat4_alloc, 0);

    rb_define_method(rb_cVector2, "each", rpg_vec2_each, 0);
    rb_define_method(rb_cVector3, "each", rpg_vec3_each, 0);
    rb_define_method(rb_cVector4, "each", rpg_vec4_each, 0);
    rb_define_method(rb_cQuaternion, "each", rpg_quat_each, 0);
    rb_define_method(rb_cMatrix3x2, "each", rpg_mat3_each, 0);
    rb_define_method(rb_cMatrix4x4, "each", rpg_mat4_each, 0);

    rb_define_method(rb_cVector2, "==", rpg_vec2_equal, 1);
    rb_define_method(rb_cVector3, "==", rpg_vec3_equal, 1);
    rb_define_method(rb_cVector4, "==", rpg_vec4_equal, 1);
    rb_define_method(rb_cQuaternion, "==", rpg_quat_equal, 1);
    rb_define_method(rb_cMatrix3x2, "==", rpg_mat3_equal, 1);
    rb_define_method(rb_cMatrix4x4, "==", rpg_mat4_equal, 1);

    rb_define_method(rb_cVector2, "inspect", rpg_vec2_inspect, 0);
    rb_define_method(rb_cVector3, "inspect", rpg_vec3_inspect, 0);
    rb_define_method(rb_cVector4, "inspect", rpg_vec4_inspect, 0);
    rb_define_method(rb_cQuaternion, "inspect", rpg_quat_inspect, 0);
    rb_define_method(rb_cMatrix3x2, "inspect", rpg_mat3_inspect, 0);
    rb_define_method(rb_cMatrix4x4, "inspect", rpg_mat4_inspect, 0);

    rb_define_singleton_method(rb_cVector2, "_load", rpg_vec2_load, 1);
    rb_define_singleton_method(rb_cVector3, "_load", rpg_vec3_load, 1);
    rb_define_singleton_method(rb_cVector4, "_load", rpg_vec4_load, 1);
    rb_define_singleton_method(rb_cQuaternion, "_load", rpg_quat_load, 1);
    rb_define_singleton_method(rb_cMatrix3x2, "_load", rpg_mat3_load, 1);
    rb_define_singleton_method(rb_cMatrix4x4, "_load", rpg_mat4_load, 1);

    rb_define_method(rb_cVector2, "_dump", rpg_vec2_dump, -1);
    rb_define_method(rb_cVector3, "_dump", rpg_vec3_dump, -1);
    rb_define_method(rb_cVector4, "_dump", rpg_vec4_dump, -1);
    rb_define_method(rb_cQuaternion, "_dump", rpg_quat_dump, -1);
    rb_define_method(rb_cMatrix3x2, "_dump", rpg_mat3_dump, -1);
    rb_define_method(rb_cMatrix4x4, "_dump", rpg_mat4_dump, -1);

    rb_define_method(rb_cVector2, "x", rpg_vec2_get_x, 0);
    rb_define_method(rb_cVector2, "y", rpg_vec2_get_y, 0);
    rb_define_method(rb_cVector2, "x=", rpg_vec2_set_x, 1);
    rb_define_method(rb_cVector2, "y=", rpg_vec2_set_y, 1);

    rb_define_method(rb_cVector3, "x", rpg_vec3_get_x, 0);
    rb_define_method(rb_cVector3, "y", rpg_vec3_get_y, 0);
    rb_define_method(rb_cVector3, "z", rpg_vec3_get_z, 0);
    rb_define_method(rb_cVector3, "x=", rpg_vec3_set_x, 1);
    rb_define_method(rb_cVector3, "y=", rpg_vec3_set_y, 1);
    rb_define_method(rb_cVector3, "z=", rpg_vec3_set_z, 1);

    rb_define_method(rb_cVector4, "x", rpg_vec4_get_x, 0);
    rb_define_method(rb_cVector4, "y", rpg_vec4_get_y, 0);
    rb_define_method(rb_cVector4, "z", rpg_vec4_get_z, 0);
    rb_define_method(rb_cVector4, "w", rpg_vec4_get_w, 0);
    rb_define_method(rb_cVector4, "x=", rpg_vec4_set_x, 1);
    rb_define_method(rb_cVector4, "y=", rpg_vec4_set_y, 1);
    rb_define_method(rb_cVector4, "z=", rpg_vec4_set_z, 1);
    rb_define_method(rb_cVector4, "w=", rpg_vec4_set_w, 1);

    rb_define_method(rb_cQuaternion, "x", rpg_quat_get_x, 0);
    rb_define_method(rb_cQuaternion, "y", rpg_quat_get_y, 0);
    rb_define_method(rb_cQuaternion, "z", rpg_quat_get_z, 0);
    rb_define_method(rb_cQuaternion, "w", rpg_quat_get_w, 0);
    rb_define_method(rb_cQuaternion, "x=", rpg_quat_set_x, 1);
    rb_define_method(rb_cQuaternion, "y=", rpg_quat_set_y, 1);
    rb_define_method(rb_cQuaternion, "z=", rpg_quat_set_z, 1);
    rb_define_method(rb_cQuaternion, "w=", rpg_quat_set_w, 1);

    rb_define_method(rb_cMatrix3x2, "m11", rpg_mat3_get_m11, 0);
    rb_define_method(rb_cMatrix3x2, "m12", rpg_mat3_get_m12, 0);
    rb_define_method(rb_cMatrix3x2, "m21", rpg_mat3_get_m21, 0);
    rb_define_method(rb_cMatrix3x2, "m22", rpg_mat3_get_m22, 0);
    rb_define_method(rb_cMatrix3x2, "m31", rpg_mat3_get_m31, 0);
    rb_define_method(rb_cMatrix3x2, "m32", rpg_mat3_get_m32, 0);
    rb_define_method(rb_cMatrix3x2, "m11=", rpg_mat3_set_m11, 1);
    rb_define_method(rb_cMatrix3x2, "m12=", rpg_mat3_set_m12, 1);
    rb_define_method(rb_cMatrix3x2, "m21=", rpg_mat3_set_m21, 1);
    rb_define_method(rb_cMatrix3x2, "m22=", rpg_mat3_set_m22, 1);
    rb_define_method(rb_cMatrix3x2, "m31=", rpg_mat3_set_m31, 1);
    rb_define_method(rb_cMatrix3x2, "m32=", rpg_mat3_set_m32, 1);

    rb_define_method(rb_cMatrix4x4, "m11", rpg_mat4_get_m11, 0);
    rb_define_method(rb_cMatrix4x4, "m12", rpg_mat4_get_m12, 0);
    rb_define_method(rb_cMatrix4x4, "m13", rpg_mat4_get_m13, 0);
    rb_define_method(rb_cMatrix4x4, "m14", rpg_mat4_get_m14, 0);
    rb_define_method(rb_cMatrix4x4, "m21", rpg_mat4_get_m21, 0);
    rb_define_method(rb_cMatrix4x4, "m22", rpg_mat4_get_m22, 0);
    rb_define_method(rb_cMatrix4x4, "m23", rpg_mat4_get_m23, 0);
    rb_define_method(rb_cMatrix4x4, "m24", rpg_mat4_get_m24, 0);
    rb_define_method(rb_cMatrix4x4, "m31", rpg_mat4_get_m31, 0);
    rb_define_method(rb_cMatrix4x4, "m32", rpg_mat4_get_m32, 0);
    rb_define_method(rb_cMatrix4x4, "m33", rpg_mat4_get_m33, 0);
    rb_define_method(rb_cMatrix4x4, "m34", rpg_mat4_get_m34, 0);
    rb_define_method(rb_cMatrix4x4, "m41", rpg_mat4_get_m41, 0);
    rb_define_method(rb_cMatrix4x4, "m42", rpg_mat4_get_m42, 0);
    rb_define_method(rb_cMatrix4x4, "m43", rpg_mat4_get_m43, 0);
    rb_define_method(rb_cMatrix4x4, "m44", rpg_mat4_get_m44, 0);
    rb_define_method(rb_cMatrix4x4, "m11=", rpg_mat4_set_m11, 1);
    rb_define_method(rb_cMatrix4x4, "m12=", rpg_mat4_set_m12, 1);
    rb_define_method(rb_cMatrix4x4, "m13=", rpg_mat4_set_m13, 1);
    rb_define_method(rb_cMatrix4x4, "m14=", rpg_mat4_set_m14, 1);
    rb_define_method(rb_cMatrix4x4, "m21=", rpg_mat4_set_m21, 1);
    rb_define_method(rb_cMatrix4x4, "m22=", rpg_mat4_set_m22, 1);
    rb_define_method(rb_cMatrix4x4, "m23=", rpg_mat4_set_m23, 1);
    rb_define_method(rb_cMatrix4x4, "m24=", rpg_mat4_set_m24, 1);
    rb_define_method(rb_cMatrix4x4, "m31=", rpg_mat4_set_m31, 1);
    rb_define_method(rb_cMatrix4x4, "m32=", rpg_mat4_set_m32, 1);
    rb_define_method(rb_cMatrix4x4, "m33=", rpg_mat4_set_m33, 1);
    rb_define_method(rb_cMatrix4x4, "m34=", rpg_mat4_set_m34, 1);
    rb_define_method(rb_cMatrix4x4, "m41=", rpg_mat4_set_m41, 1);
    rb_define_method(rb_cMatrix4x4, "m42=", rpg_mat4_set_m42, 1);
    rb_define_method(rb_cMatrix4x4, "m43=", rpg_mat4_set_m43, 1);
    rb_define_method(rb_cMatrix4x4, "m44=", rpg_mat4_set_m44, 1);
}

ALLOC_FUNC(rpg_vec2_alloc, RPGvector2)
ALLOC_FUNC(rpg_vec3_alloc, RPGvector3)
ALLOC_FUNC(rpg_vec4_alloc, RPGvector4)
ALLOC_FUNC(rpg_quat_alloc, RPGvector4)
ALLOC_FUNC(rpg_mat3_alloc, RPGmatrix3x2)
ALLOC_FUNC(rpg_mat4_alloc, RPGmatrix4x4)

EACH_FUNC(rpg_vec2_each, RPGvector2)
EACH_FUNC(rpg_vec3_each, RPGvector3)
EACH_FUNC(rpg_vec4_each, RPGvector4)
EACH_FUNC(rpg_quat_each, RPGvector4)
EACH_FUNC(rpg_mat3_each, RPGmatrix3x2)
EACH_FUNC(rpg_mat4_each, RPGmatrix4x4)

EQUAL_FUNC(rpg_vec2_equal, RPGvector2)
EQUAL_FUNC(rpg_vec3_equal, RPGvector3)
EQUAL_FUNC(rpg_vec4_equal, RPGvector4)
EQUAL_FUNC(rpg_quat_equal, RPGvector4)
EQUAL_FUNC(rpg_mat3_equal, RPGmatrix3x2)
EQUAL_FUNC(rpg_mat4_equal, RPGmatrix4x4)

DUP_FUNC(rpg_vec2_dup, RPGvector2)
DUP_FUNC(rpg_vec3_dup, RPGvector3)
DUP_FUNC(rpg_vec4_dup, RPGvector4)
DUP_FUNC(rpg_quat_dup, RPGvector4)
DUP_FUNC(rpg_mat3_dup, RPGmatrix3x2)
DUP_FUNC(rpg_mat4_dup, RPGmatrix4x4)

DUMP_FUNC(rpg_vec2_dump, RPGvector2)
DUMP_FUNC(rpg_vec3_dump, RPGvector3)
DUMP_FUNC(rpg_vec4_dump, RPGvector4)
DUMP_FUNC(rpg_quat_dump, RPGvector4)
DUMP_FUNC(rpg_mat3_dump, RPGmatrix3x2)
DUMP_FUNC(rpg_mat4_dump, RPGmatrix4x4)

LOAD_FUNC(rpg_vec2_load, RPGvector2)
LOAD_FUNC(rpg_vec3_load, RPGvector3)
LOAD_FUNC(rpg_vec4_load, RPGvector4)
LOAD_FUNC(rpg_quat_load, RPGvector4)
LOAD_FUNC(rpg_mat3_load, RPGmatrix3x2)
LOAD_FUNC(rpg_mat4_load, RPGmatrix4x4)

ATTR_READER(rpg_vec2_get_x, RPGvector2, x, DBL2NUM)
ATTR_READER(rpg_vec2_get_y, RPGvector2, y, DBL2NUM)
ATTR_WRITER(rpg_vec2_set_x, RPGvector2, x, NUM2FLT)
ATTR_WRITER(rpg_vec2_set_y, RPGvector2, y, NUM2FLT)

ATTR_READER(rpg_vec3_get_x, RPGvector3, x, DBL2NUM)
ATTR_READER(rpg_vec3_get_y, RPGvector3, y, DBL2NUM)
ATTR_READER(rpg_vec3_get_z, RPGvector3, z, DBL2NUM)
ATTR_WRITER(rpg_vec3_set_x, RPGvector3, x, NUM2FLT)
ATTR_WRITER(rpg_vec3_set_y, RPGvector3, y, NUM2FLT)
ATTR_WRITER(rpg_vec3_set_z, RPGvector3, z, NUM2FLT)

ATTR_READER(rpg_vec4_get_x, RPGvector4, x, DBL2NUM)
ATTR_READER(rpg_vec4_get_y, RPGvector4, y, DBL2NUM)
ATTR_READER(rpg_vec4_get_z, RPGvector4, z, DBL2NUM)
ATTR_READER(rpg_vec4_get_w, RPGvector4, w, DBL2NUM)
ATTR_WRITER(rpg_vec4_set_x, RPGvector4, x, NUM2FLT)
ATTR_WRITER(rpg_vec4_set_y, RPGvector4, y, NUM2FLT)
ATTR_WRITER(rpg_vec4_set_z, RPGvector4, z, NUM2FLT)
ATTR_WRITER(rpg_vec4_set_w, RPGvector4, w, NUM2FLT)

ATTR_READER(rpg_quat_get_x, RPGvector4, x, DBL2NUM)
ATTR_READER(rpg_quat_get_y, RPGvector4, y, DBL2NUM)
ATTR_READER(rpg_quat_get_z, RPGvector4, z, DBL2NUM)
ATTR_READER(rpg_quat_get_w, RPGvector4, w, DBL2NUM)
ATTR_WRITER(rpg_quat_set_x, RPGvector4, x, NUM2FLT)
ATTR_WRITER(rpg_quat_set_y, RPGvector4, y, NUM2FLT)
ATTR_WRITER(rpg_quat_set_z, RPGvector4, z, NUM2FLT)
ATTR_WRITER(rpg_quat_set_w, RPGvector4, w, NUM2FLT)

ATTR_READER(rpg_mat3_get_m11, RPGmatrix4x4, m11, DBL2NUM)
ATTR_READER(rpg_mat3_get_m12, RPGmatrix4x4, m12, DBL2NUM)
ATTR_READER(rpg_mat3_get_m21, RPGmatrix4x4, m21, DBL2NUM)
ATTR_READER(rpg_mat3_get_m22, RPGmatrix4x4, m22, DBL2NUM)
ATTR_READER(rpg_mat3_get_m31, RPGmatrix4x4, m31, DBL2NUM)
ATTR_READER(rpg_mat3_get_m32, RPGmatrix4x4, m32, DBL2NUM)
ATTR_WRITER(rpg_mat3_set_m11, RPGmatrix4x4, m11, NUM2FLT)
ATTR_WRITER(rpg_mat3_set_m12, RPGmatrix4x4, m12, NUM2FLT)
ATTR_WRITER(rpg_mat3_set_m21, RPGmatrix4x4, m21, NUM2FLT)
ATTR_WRITER(rpg_mat3_set_m22, RPGmatrix4x4, m22, NUM2FLT)
ATTR_WRITER(rpg_mat3_set_m31, RPGmatrix4x4, m31, NUM2FLT)
ATTR_WRITER(rpg_mat3_set_m32, RPGmatrix4x4, m32, NUM2FLT)

ATTR_READER(rpg_mat4_get_m11, RPGmatrix4x4, m11, DBL2NUM)
ATTR_READER(rpg_mat4_get_m12, RPGmatrix4x4, m12, DBL2NUM)
ATTR_READER(rpg_mat4_get_m13, RPGmatrix4x4, m13, DBL2NUM)
ATTR_READER(rpg_mat4_get_m14, RPGmatrix4x4, m14, DBL2NUM)
ATTR_READER(rpg_mat4_get_m21, RPGmatrix4x4, m21, DBL2NUM)
ATTR_READER(rpg_mat4_get_m22, RPGmatrix4x4, m22, DBL2NUM)
ATTR_READER(rpg_mat4_get_m23, RPGmatrix4x4, m23, DBL2NUM)
ATTR_READER(rpg_mat4_get_m24, RPGmatrix4x4, m24, DBL2NUM)
ATTR_READER(rpg_mat4_get_m31, RPGmatrix4x4, m31, DBL2NUM)
ATTR_READER(rpg_mat4_get_m32, RPGmatrix4x4, m32, DBL2NUM)
ATTR_READER(rpg_mat4_get_m33, RPGmatrix4x4, m33, DBL2NUM)
ATTR_READER(rpg_mat4_get_m34, RPGmatrix4x4, m34, DBL2NUM)
ATTR_READER(rpg_mat4_get_m41, RPGmatrix4x4, m41, DBL2NUM)
ATTR_READER(rpg_mat4_get_m42, RPGmatrix4x4, m42, DBL2NUM)
ATTR_READER(rpg_mat4_get_m43, RPGmatrix4x4, m43, DBL2NUM)
ATTR_READER(rpg_mat4_get_m44, RPGmatrix4x4, m44, DBL2NUM)
ATTR_WRITER(rpg_mat4_set_m11, RPGmatrix4x4, m11, NUM2FLT)
ATTR_WRITER(rpg_mat4_set_m12, RPGmatrix4x4, m12, NUM2FLT)
ATTR_WRITER(rpg_mat4_set_m13, RPGmatrix4x4, m13, NUM2FLT)
ATTR_WRITER(rpg_mat4_set_m14, RPGmatrix4x4, m14, NUM2FLT)
ATTR_WRITER(rpg_mat4_set_m21, RPGmatrix4x4, m21, NUM2FLT)
ATTR_WRITER(rpg_mat4_set_m22, RPGmatrix4x4, m22, NUM2FLT)
ATTR_WRITER(rpg_mat4_set_m23, RPGmatrix4x4, m23, NUM2FLT)
ATTR_WRITER(rpg_mat4_set_m24, RPGmatrix4x4, m24, NUM2FLT)
ATTR_WRITER(rpg_mat4_set_m31, RPGmatrix4x4, m31, NUM2FLT)
ATTR_WRITER(rpg_mat4_set_m32, RPGmatrix4x4, m32, NUM2FLT)
ATTR_WRITER(rpg_mat4_set_m33, RPGmatrix4x4, m33, NUM2FLT)
ATTR_WRITER(rpg_mat4_set_m34, RPGmatrix4x4, m34, NUM2FLT)
ATTR_WRITER(rpg_mat4_set_m41, RPGmatrix4x4, m41, NUM2FLT)
ATTR_WRITER(rpg_mat4_set_m42, RPGmatrix4x4, m42, NUM2FLT)
ATTR_WRITER(rpg_mat4_set_m43, RPGmatrix4x4, m43, NUM2FLT)
ATTR_WRITER(rpg_mat4_set_m44, RPGmatrix4x4, m44, NUM2FLT)

static VALUE rpg_vec2_initialize(int argc, VALUE *argv, VALUE self) {
    VALUE a1, a2;
    rb_scan_args(argc, argv, "02", &a1, &a2);
    RPGvector2 *v = DATA_PTR(self);
    switch (argc) {
        case 1: {
            if (RB_TYPE_P(a1, T_DATA)) {
                float *other = DATA_PTR(a1);
                memcpy(v, other, sizeof(RPGvector2));
            }
            else {
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
    RPGvector3 *v = DATA_PTR(self);
    switch (argc) {
        case 1: {
            if (RB_TYPE_P(a1, T_DATA)) {
                float *other = DATA_PTR(a1);
                memcpy(v, other, sizeof(RPGvector3));
            }
            else {
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
            }
            else {
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
    RPGvector4 *v = DATA_PTR(self);

    switch (argc) {
        case 1: {
            if (RB_TYPE_P(a1, T_DATA)) {
                float *other = DATA_PTR(a1);
                memcpy(v, other, sizeof(RPGvector4));
            }
            else {
                float f = NUM2FLT(a1);
                VEC4_SET(v, f, f, f, f);
            }
            break;
        }
        case 2: {
            if (RB_IS_A(a1, rb_cVector2)) {
                RPGvector2 *v1 = DATA_PTR(a1);
                RPGvector2 *v2 = DATA_PTR(a2);
                VEC4_SET(v, v1->x, v1->y, v2->x, v2->y);
            }
            else if (RB_IS_A(a1, rb_cVector3)) {
                RPGvector3 *vec3 = DATA_PTR(a1);
                VEC4_SET(v, vec3->x, vec3->y, vec3->z, NUM2FLT(a2));
            }
            else {
                RPGvector3 *vec3 = DATA_PTR(a2);
                VEC4_SET(v, NUM2FLT(a1), vec3->x, vec3->y, vec3->z);
            }
            break;
        }
        case 3: {
            RPGvector2 *vec2;
            if (RB_IS_A(a1, rb_cVector2)) {
                vec2 = DATA_PTR(a1);
                VEC4_SET(v, vec2->x, vec2->y, NUM2FLT(a2), NUM2FLT(a3));
            }
            else if (RB_IS_A(a2, rb_cVector2)) {
                vec2 = DATA_PTR(a2);
                VEC4_SET(v, NUM2FLT(a1), vec2->x, vec2->y, NUM2FLT(a3));
            }
            else {
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
            mat[i] = NUM2FLT(argv[i]);
        }
    }
    return Qnil;
}

static VALUE rpg_mat4_initialize(int argc, VALUE *argv, VALUE self) {
    switch (argc) {
        case 0:
            break;
        case 1: {
            if (RB_IS_A(argv[0], rb_cMatrix3x2)) {
                rb_raise(rb_eTypeError, "expected Matrix3x2");
            }
            RPGmatrix4x4 *mat4 = DATA_PTR(self);
            RPGmatrix3x2 *mat3 = DATA_PTR(argv[0]);
            MAT4_SET(mat4, mat3->m11, mat3->m12, 0.0f, 0.0f, mat3->m21, mat3->m22, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, mat3->m31, mat3->m32, 0.0f, 1.0f);
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
    RPGvector4 *v = DATA_PTR(self);
    return rb_sprintf("<Vector2: x:%f y:%f>", v->x, v->y);
}

static VALUE rpg_vec3_inspect(VALUE self) {
    RPGvector3 *v = DATA_PTR(self);
    return rb_sprintf("<Vector3: x:%f y:%f z:%f>", v->x, v->y, v->z);
}

static VALUE rpg_vec4_inspect(VALUE self) {
    RPGvector4 *v = DATA_PTR(self);
    return rb_sprintf("<Vector4: x:%f y:%f z:%f w:%f>", v->x, v->y, v->z, v->w);
}

static VALUE rpg_quat_inspect(VALUE self) {
    RPGvector4 *v = DATA_PTR(self);
    return rb_sprintf("<Quaternion: x:%f y:%f z:%f w:%f>", v->x, v->y, v->z, v->w);
}

static VALUE rpg_mat3_inspect(VALUE self) {
    RPGmatrix3x2 *m = DATA_PTR(self);
    return rb_sprintf("<Matrix4x4: %f, %f, %f, %f, %f, %f>", m->m11, m->m12, m->m21, m->m22, m->m31, m->m32);
}

static VALUE rpg_mat4_inspect(VALUE self) {
    RPGmatrix4x4 *m = DATA_PTR(self);
    return rb_sprintf("<Matrix4x4: %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f>", 
        m->m11, m->m12, m->m13, m->m14,
        m->m21, m->m22, m->m23, m->m24,
        m->m31, m->m32, m->m33, m->m34,
        m->m41, m->m42, m->m43, m->m44
    );
}












void rpg_mat4_create_ortho(RPGmatrix4x4 *mat4, float left, float right, float bottom, float top, float near, float far) {

    memset(mat4, 0, sizeof(RPGmatrix4x4));

    mat4->m11 = 2.0f / (right - left);
    mat4->m12 = mat4->m13 = mat4->m14 = 0.0f;

    mat4->m22 = 2.0f / (top - bottom);
    mat4->m21 = mat4->m23 = mat4->m24 = 0.0f;

    mat4->m33 = 1.0f / (near - far);
    mat4->m31 = mat4->m32 = mat4->m34 = 0.0f;

    mat4->m41 = (left + right) / (left - right);
    mat4->m42 = (top + bottom) / (bottom - top);
    mat4->m43 = near / (near - far);
    mat4->m44 = 1.0f;
}

