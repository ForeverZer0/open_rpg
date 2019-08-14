#ifndef OPEN_RGP_INTERNAL_H
#define OPEN_RGP_INTERNAL_H 1

#include "./rpg.h"

#define VERTICES_COUNT 24
#define VERTICES_SIZE (sizeof(float) * VERTICES_COUNT)
#define VERTICES_STRIDE (sizeof(float) * 4)

#define DUMP_FUNC(function, type)                                                                                                          \
    static VALUE function(int argc, VALUE *argv, VALUE self) {                                                                             \
        void *obj = DATA_PTR(self);                                                                                                        \
        return rb_str_new(obj, sizeof(type));                                                                                              \
    }

#define LOAD_FUNC(function, type)                                                                                                          \
    static VALUE function(VALUE klass, VALUE str) {                                                                                        \
        type *obj = ALLOC(type);                                                                                                           \
        void *src = StringValueCStr(str);                                                                                                  \
        memcpy(obj, src, sizeof(type));                                                                                                    \
        return Data_Wrap_Struct(klass, NULL, RUBY_DEFAULT_FREE, obj);                                                                      \
    }

#define DUP_FUNC(function, type)                                                                                                           \
    static VALUE function(VALUE self) {                                                                                                    \
        type *src = DATA_PTR(self);                                                                                                        \
        type *dst = ALLOC(type);                                                                                                           \
        memcpy(dst, src, sizeof(type));                                                                                                    \
        return Data_Wrap_Struct(CLASS_OF(self), NULL, RUBY_DEFAULT_FREE, dst);                                                             \
    }

#define ATTR_READER(function, type, field, macro)                                                                                          \
    static VALUE function(VALUE self) {                                                                                                    \
        type *obj = DATA_PTR(self);                                                                                                        \
        return macro(obj->field);                                                                                                          \
    }

#define ATTR_WRITER(function, type, field, macro)                                                                                          \
    static VALUE function(VALUE self, VALUE value) {                                                                                       \
        type *obj = DATA_PTR(self);                                                                                                        \
        obj->field = macro(value);                                                                                                         \
        return value;                                                                                                                      \
    }

#define ALLOC_FUNC(function, type)                                                                                                         \
    static VALUE function(VALUE klass) {                                                                                                   \
        type *value = ALLOC(type);                                                                                                         \
        memset(value, 0, sizeof(type));                                                                                                    \
        return Data_Wrap_Struct(klass, NULL, RUBY_DEFAULT_FREE, value);                                                                    \
    }


#endif /* OPEN_RGP_INTERNAL_H */