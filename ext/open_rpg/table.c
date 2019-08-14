#include "./table.h"

VALUE rb_cTable;

#define CHECK_DIMENSION(v)                                                                                                                 \
    if (v < 1) {                                                                                                                           \
        rb_raise(rb_eArgError, "dimensions must be greater than 0 (given %d)", v);                                                         \
    }

#define CHECK_BOUNDS(num, tbl, field)                                                                                                      \
    if (num < 0 || num >= tbl->field)                                                                                                      \
        rb_raise(rb_eRangeError, "index out of bounds");

void rpg_table_init(VALUE parent) {
    rb_cTable = rb_define_class_under(parent, "Table", rb_cObject);
    rb_define_alloc_func(rb_cTable, rpg_table_alloc);

    rb_define_method(rb_cTable, "initialize", rpg_table_initialize, -1);
    rb_define_method(rb_cTable, "[]", rpg_table_get_value, -1);
    rb_define_method(rb_cTable, "[]=", rpg_table_set_value, -1);
    rb_define_method(rb_cTable, "resize", rpg_table_resize, -1);
    rb_define_method(rb_cTable, "width", rpg_table_width, 0);
    rb_define_method(rb_cTable, "height", rpg_table_height, 0);
    rb_define_method(rb_cTable, "depth", rpg_table_depth, 0);
    rb_define_method(rb_cTable, "inspect", rpg_table_inspect, 0);
    rb_define_method(rb_cTable, "dup", rpg_table_dup, 0);
    rb_define_method(rb_cTable, "dimensions", rpg_table_dimensions, 0);
    rb_define_method(rb_cTable, "clear", rpg_table_clear, 0);

    rb_include_module(rb_cTable, rb_mEnumerable);
    rb_define_method(rb_cTable, "each", rpg_table_each, 0);
    rb_define_method(rb_cTable, "length", rpg_table_length, 0);
    rb_define_alias(rb_cTable, "size", "length");

    rb_define_alias(rb_cTable, "xsize", "width");
    rb_define_alias(rb_cTable, "ysize", "height");
    rb_define_alias(rb_cTable, "zsize", "depth");
}

ALLOC_FUNC(rpg_table_alloc, RPGtable)

void rpg_table_free(void *data) {
    RPGtable *table = DATA_PTR(data);
    if (table->data) {
        RPG_FREE(table->data);
    }
    RPG_FREE(data);
}

static VALUE rpg_table_initialize(int argc, VALUE *argv, VALUE self) {
    RPGtable *t = DATA_PTR(self);
    VALUE w, h, d;
    rb_scan_args(argc, argv, "12", &w, &h, &d);

    size_t size;
    t->dims = (GLbyte)argc;
    t->width = NUM2INT(w);
    CHECK_DIMENSION(t->width)
    size = t->width;
    if (argc > 1) {
        t->height = NUM2INT(h);
        CHECK_DIMENSION(t->height)
        size *= t->height;
        if (argc > 2) {
            t->depth = NUM2INT(d);
            CHECK_DIMENSION(t->depth)
            size *= t->depth;
        }
    }
    size *= sizeof(short);
    t->data = RPG_ALLOC(size);
    memset(t->data, 0, size);
    return Qnil;
}

static VALUE rpg_table_get_value(int argc, VALUE *argv, VALUE self) {
    RPGtable *t = DATA_PTR(self);
    if (argc != t->dims) {
        rb_raise(rb_eArgError, "wrong number of arguments (given %d, expected %d)", argc, t->dims);
    }
    VALUE x, y, z;
    rb_scan_args(argc, argv, "12", &x, &y, &z);
    int v, i = NUM2INT(x);
    CHECK_BOUNDS(i, t, width)
    if (argc > 1) {
        v = NUM2INT(y);
        CHECK_BOUNDS(v, t, height)
        i += (v * t->width);
        if (argc > 2) {
            v = NUM2INT(z);
            CHECK_BOUNDS(v, t, depth)
            i += (v * t->width * t->depth);
        }
    }
    return INT2NUM(t->data[i]);
}

static VALUE rpg_table_set_value(int argc, VALUE *argv, VALUE self) {
    RPGtable *t = DATA_PTR(self);
    if (argc != t->dims + 1) {
        rb_raise(rb_eArgError, "wrong number of arguments (given %d, expected %d)", argc, t->dims + 1);
    }
    VALUE x, y, z, w;
    rb_scan_args(argc, argv, "22", &x, &y, &z, &w);
    int v, i = NUM2INT(x);
    if (argc == 2) {
        CHECK_BOUNDS(i, t, width)
        t->data[i] = NUM2SHORT(y);
        return y;
    } else if (argc == 3) {
        v = NUM2INT(y);
        CHECK_BOUNDS(v, t, height);
        i += (v * t->width);
        t->data[i] = NUM2SHORT(z);
        return z;
    } else {
        v = NUM2INT(y);
        int v2 = NUM2INT(z);
        CHECK_BOUNDS(v, t, height)
        CHECK_BOUNDS(v2, t, depth)
        i += (v * t->width) + (v2 * t->width * t->height);
        t->data[i] = NUM2SHORT(w);
        return w;
    }
}

static VALUE rpg_table_width(VALUE self) {
    RPGtable *t = DATA_PTR(self);
    return INT2NUM(t->width);
}

static VALUE rpg_table_height(VALUE self) {
    RPGtable *t = DATA_PTR(self);
    return INT2NUM(t->height);
}

static VALUE rpg_table_depth(VALUE self) {
    RPGtable *t = DATA_PTR(self);
    return INT2NUM(t->depth);
}

static VALUE rpg_table_dimensions(VALUE self) {
    RPGtable *t = DATA_PTR(self);
    return INT2NUM(t->dims);
}

static VALUE rpg_table_resize(int argc, VALUE *argv, VALUE self) {
    RPGtable *t = DATA_PTR(self);
    RPGtable *temp = ALLOC(RPGtable);

    VALUE w, h, d;
    rb_scan_args(argc, argv, "12", &w, &h, &d);

    size_t size;
    temp->dims = (GLbyte)argc;
    temp->width = NUM2INT(w);
    CHECK_DIMENSION(temp->width)
    size = temp->width;
    if (argc > 1) {
        temp->height = NUM2INT(h);
        CHECK_DIMENSION(temp->height)
        size *= temp->height;
        if (argc > 2) {
            temp->depth = NUM2INT(d);
            CHECK_DIMENSION(temp->depth)
            size *= temp->depth;
        }
    }
    size *= sizeof(short);
    temp->data = RPG_ALLOC(size);
    memset(temp->data, 0, size);

    int max_x = imin(temp->width, t->width);
    int max_y = imin(temp->height, t->height);
    int max_z = imin(temp->depth, t->depth);

    for (int x = 0; x < max_x; x++) {
        if (max_y > 0) {
            for (int y = 0; y < max_y; y++) {
                if (max_z > 0) {
                    for (int z = 0; z < max_z; z++) {
                        temp->data[x + (y * temp->width) + (z * temp->width * temp->height)] =
                            t->data[x + (y * t->width) + (z * t->width * t->height)];
                    }
                } else {
                    temp->data[x + (y * temp->width)] = t->data[x + (y * t->width)];
                }
            }
        } else {
            temp->data[x] = t->data[x];
        }
    }
    RPG_FREE(t->data);
    memcpy(t, temp, sizeof(RPGtable));
    return self;
}

static VALUE rpg_table_dup(VALUE self) {
    RPGtable *t = DATA_PTR(self);
    RPGtable *clone = ALLOC(RPGtable);
    memcpy(clone, t, sizeof(RPGtable));

    size_t size = t->width;
    if (t->dims > 1) {
        size *= t->height;
        if (t->dims > 2) {
            size *= t->depth;
        }
    }
    size *= sizeof(short);
    clone->data = RPG_ALLOC(size);
    memcpy(clone->data, t->data, size);
    return Data_Wrap_Struct(CLASS_OF(self), NULL, rpg_table_free, clone);
}

static VALUE rpg_table_inspect(VALUE self) {
    RPGtable *t = DATA_PTR(self);
    return rb_sprintf("<Table: width:%d, height:%d depth:%d>", t->width, t->height, t->depth);
}

static VALUE rpg_table_enum_length(VALUE table, VALUE args, VALUE eobj) { return rpg_table_length(table); }

static inline int rpg_table_length_inline(RPGtable *table) {
    int len = table->width;
    if (table->dims > 1) {
        len *= table->height;
        if (table->dims) {
            len *= table->depth;
        }
    }
    return len;
}

static VALUE rpg_table_length(VALUE self) {
    RPGtable *t = DATA_PTR(self);
    int len = rpg_table_length_inline(t);
    return INT2NUM(len);
}

static VALUE rpg_table_clear(VALUE self) {
    RPGtable *t = DATA_PTR(self);
    size_t len = sizeof(short) * rpg_table_length_inline(t);
    memset(t->data, 0, len);
    return Qnil;
}

static VALUE rpg_table_each(VALUE self) {
    RETURN_SIZED_ENUMERATOR(self, 0, 0, rpg_table_enum_length);

    RPGtable *t = DATA_PTR(self);
    int len = rpg_table_length_inline(t);
    for (int i = 0; i < len; i++) {
        rb_yield(INT2NUM(t->data[i]));
    }
    return self;
}