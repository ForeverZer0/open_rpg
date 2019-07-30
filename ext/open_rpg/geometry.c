#include "./geometry.h"

VALUE rb_cPoint;
VALUE rb_cSize;
VALUE rb_cRect;

void rpg_geometry_init(VALUE parent) {
    rb_cPoint = rb_define_class_under(parent, "Point", rb_cObject);
    rb_cSize = rb_define_class_under(parent, "Size", rb_cObject);
    rb_cRect = rb_define_class_under(parent, "Rect", rb_cObject);

    rb_define_alloc_func(rb_cPoint, rpg_point_alloc);
    rb_define_alloc_func(rb_cSize, rpg_size_alloc);
    rb_define_alloc_func(rb_cRect, rpg_rect_alloc);

    rb_define_singleton_method(rb_cPoint, "empty", rpg_point_alloc, 0);
    rb_define_singleton_method(rb_cPoint, "_load", rpg_point_load, 1);
    rb_define_method(rb_cPoint, "initialize", rpg_point_initialize, -1);
    rb_define_method(rb_cPoint, "set", rpg_point_set, 2);
    rb_define_method(rb_cPoint, "dup", rpg_point_dup, 0);
    rb_define_method(rb_cPoint, "empty?", rpg_point_empty_p, 0);
    rb_define_method(rb_cPoint, "_dump", rpg_point_dump, -1);
    rb_define_method(rb_cPoint, "inspect", rpg_point_inspect, 0);
    rb_define_method(rb_cPoint, "x", rpg_point_get_x, 0);
    rb_define_method(rb_cPoint, "y", rpg_point_get_y, 0);
    rb_define_method(rb_cPoint, "x=", rpg_point_set_x, 1);
    rb_define_method(rb_cPoint, "y=", rpg_point_set_y, 1);
    rb_define_method(rb_cPoint, "add", rpg_point_add, 1);
    rb_define_method(rb_cPoint, "minus", rpg_point_minus, 1);
    rb_define_method(rb_cPoint, "+", rpg_point_add, 1);
    rb_define_method(rb_cPoint, "-", rpg_point_minus, 1);
    rb_define_method(rb_cPoint, "offset", rpg_point_offset, -1);
    rb_define_method(rb_cPoint, "offset!", rpg_point_offset_bang, -1);
    rb_define_method(rb_cPoint, "==", rpg_point_equal, 1);

    rb_define_singleton_method(rb_cSize, "empty", rpg_size_alloc, 0);
    rb_define_singleton_method(rb_cSize, "_load", rpg_size_load, 1);
    rb_define_method(rb_cSize, "initialize", rpg_size_initialize, -1);
    rb_define_method(rb_cSize, "set", rpg_size_set, 2);
    rb_define_method(rb_cSize, "dup", rpg_size_dup, 0);
    rb_define_method(rb_cSize, "empty?", rpg_size_empty_p, 0);
    rb_define_method(rb_cSize, "_dump", rpg_size_dump, -1);
    rb_define_method(rb_cSize, "width", rpg_size_get_width, 0);
    rb_define_method(rb_cSize, "height", rpg_size_get_height, 0);
    rb_define_method(rb_cSize, "width=", rpg_size_set_width, 1);
    rb_define_method(rb_cSize, "height=", rpg_size_set_height, 1);
    rb_define_method(rb_cSize, "inspect", rpg_size_inspect, 0);
    rb_define_method(rb_cSize, "add", rpg_size_add, 1);
    rb_define_method(rb_cSize, "minus", rpg_size_minus, 1);
    rb_define_method(rb_cSize, "+", rpg_size_add, 1);
    rb_define_method(rb_cSize, "-", rpg_size_minus, 1);
    rb_define_method(rb_cSize, "==", rpg_size_equal, 1);

    rb_define_singleton_method(rb_cRect, "from_ltrb", rpg_rect_from_ltrb, 4);
    rb_define_singleton_method(rb_cRect, "_load", rpg_rect_load, 1);
    rb_define_singleton_method(rb_cRect, "empty", rpg_rect_alloc, 0);
    rb_define_method(rb_cRect, "initialize", rpg_rect_initialize, -1);
    rb_define_method(rb_cRect, "set", rpg_rect_set, 4);
    rb_define_method(rb_cRect, "dup", rpg_rect_dup, 0);
    rb_define_method(rb_cRect, "empty?", rpg_rect_empty_p, 0);
    rb_define_method(rb_cRect, "_dump", rpg_rect_dump, -1);
    rb_define_method(rb_cRect, "inspect", rpg_rect_inspect, 0);
    rb_define_method(rb_cRect, "x", rpg_rect_get_x, 0);
    rb_define_method(rb_cRect, "y", rpg_rect_get_y, 0);
    rb_define_method(rb_cRect, "width", rpg_rect_get_width, 0);
    rb_define_method(rb_cRect, "height", rpg_rect_get_height, 0);
    rb_define_method(rb_cRect, "x=", rpg_rect_set_x, 1);
    rb_define_method(rb_cRect, "y=", rpg_rect_set_y, 1);
    rb_define_method(rb_cRect, "width=", rpg_rect_set_width, 1);
    rb_define_method(rb_cRect, "height=", rpg_rect_set_height, 1);
    rb_define_method(rb_cRect, "left", rpg_rect_get_x, 0);
    rb_define_method(rb_cRect, "top", rpg_rect_get_y, 0);
    rb_define_method(rb_cRect, "right", rpg_rect_right, 0);
    rb_define_method(rb_cRect, "bottom", rpg_rect_bottom, 0);
    rb_define_method(rb_cRect, "area", rpg_rect_area, 0);
    rb_define_method(rb_cRect, "perimeter", rpg_rect_perimeter, 0);
    rb_define_method(rb_cRect, "size", rpg_rect_size, 0);
    rb_define_method(rb_cRect, "location", rpg_rect_location, 0);
    rb_define_method(rb_cRect, "include?", rpg_rect_include_p, -1);
    rb_define_method(rb_cRect, "inflate", rpg_rect_inflate, -1);
    rb_define_method(rb_cRect, "inflate!", rpg_rect_inflate_bang, -1);
    rb_define_method(rb_cRect, "intersect", rpg_rect_intersect, 1);
    rb_define_method(rb_cRect, "intersect!", rpg_rect_intersect_bang, 1);
    rb_define_method(rb_cRect, "intersects?", rpg_rect_intersects_p, 1);
    rb_define_method(rb_cRect, "union", rpg_rect_union, 1);
    rb_define_method(rb_cRect, "union!", rpg_rect_union_bang, 1);
    rb_define_method(rb_cRect, "offset", rpg_rect_offset, -1);
    rb_define_method(rb_cRect, "offset!", rpg_rect_offset_bang, -1);
    rb_define_method(rb_cRect, "==", rpg_rect_equal, 1);
}

ALLOC_FUNC(rpg_point_alloc, RPGpoint)
ALLOC_FUNC(rpg_size_alloc, RPGsize)
ALLOC_FUNC(rpg_rect_alloc, RPGrect)

DUP_FUNC(rpg_point_dup, RPGpoint)
DUP_FUNC(rpg_size_dup, RPGsize)
DUP_FUNC(rpg_rect_dup, RPGrect)

DUMP_FUNC(rpg_point_dump, RPGpoint)
DUMP_FUNC(rpg_size_dump, RPGsize)
DUMP_FUNC(rpg_rect_dump, RPGrect)

LOAD_FUNC(rpg_point_load, RPGpoint)
LOAD_FUNC(rpg_size_load, RPGsize)
LOAD_FUNC(rpg_rect_load, RPGrect)

ATTR_READER(rpg_point_get_x, RPGpoint, x, INT2NUM)
ATTR_READER(rpg_point_get_y, RPGpoint, y, INT2NUM)
ATTR_READER(rpg_size_get_width, RPGsize, width, INT2NUM)
ATTR_READER(rpg_size_get_height, RPGsize, height, INT2NUM)
ATTR_READER(rpg_rect_get_x, RPGrect, x, INT2NUM)
ATTR_READER(rpg_rect_get_y, RPGrect, y, INT2NUM)
ATTR_READER(rpg_rect_get_width, RPGrect, width, INT2NUM)
ATTR_READER(rpg_rect_get_height, RPGrect, height, INT2NUM)

ATTR_WRITER(rpg_point_set_x, RPGpoint, x, NUM2INT)
ATTR_WRITER(rpg_point_set_y, RPGpoint, y, NUM2INT)
ATTR_WRITER(rpg_size_set_width, RPGsize, width, NUM2INT)
ATTR_WRITER(rpg_size_set_height, RPGsize, height, NUM2INT)
ATTR_WRITER(rpg_rect_set_x, RPGrect, x, NUM2INT)
ATTR_WRITER(rpg_rect_set_y, RPGrect, y, NUM2INT)
ATTR_WRITER(rpg_rect_set_width, RPGrect, width, NUM2INT)
ATTR_WRITER(rpg_rect_set_height, RPGrect, height, NUM2INT)

static VALUE rpg_point_initialize(int argc, VALUE *argv, VALUE self) {
    VALUE arg1, arg2;
    rb_scan_args(argc, argv, "02", &arg1, &arg2);
    if (argc > 0) {
        RPGpoint *point = DATA_PTR(self);
        if (argc == 1) {
            if (RB_TYPE_P(arg1, T_DATA)) {
                RPGsize *size = DATA_PTR(arg1);
                memcpy(point, size, sizeof(RPGpoint));
            }
            else {
                point->x = NUM2INT(arg1);
                point->y = point->x;
            }
        }
        else if (argc == 2) {
            point->x = NUM2INT(arg1);
            point->y = NUM2INT(arg2);
        }
    }
    return Qnil;
}

static VALUE rpg_size_initialize(int argc, VALUE *argv, VALUE self) {
    VALUE arg1, arg2;
    rb_scan_args(argc, argv, "02", &arg1, &arg2);
    if (argc > 0) {
        RPGsize *size = DATA_PTR(self);
        if (argc == 1) {
            if (RB_TYPE_P(arg1, T_DATA)) {
                RPGpoint *point = DATA_PTR(arg1);
                memcpy(size, point, sizeof(RPGpoint));
            }
            else {
                size->width = NUM2INT(arg1);
                size->height = size->width;
            }
        }
        else if (argc == 2) {
            size->width = NUM2INT(arg1);
            size->height = NUM2INT(arg2);
        }
    }
    return Qnil;
}

static VALUE rpg_rect_initialize(int argc, VALUE *argv, VALUE self) {
    VALUE arg1, arg2, arg3, arg4;
    rb_scan_args(argc, argv, "04", &arg1, &arg2, &arg3, &arg4);
    RPGrect *rect = DATA_PTR(self);
    switch (argc) {
        case 1: {
            RPGrect *other = DATA_PTR(arg1);
            memcpy(rect, other, sizeof(RPGrect));
            break;
        }
        case 2: {
            RPGpoint *point = DATA_PTR(arg1);
            RPGsize *size = DATA_PTR(arg2);
            rect->x = point->x;
            rect->y = point->y;
            rect->width = size->width;
            rect->height = size->height;
            break;
        }
        case 3: {
            if (RB_TYPE_P(arg1, T_DATA)) {
                RPGpoint *p = DATA_PTR(arg1);
                rect->x = p->x;
                rect->y = p->y;
                rect->width = NUM2INT(arg2);
                rect->height = NUM2INT(arg3);
                break;
            }
            else {
                RPGsize *s = DATA_PTR(arg3);
                rect->x = NUM2INT(arg1);
                rect->y = NUM2INT(arg2);
                rect->width = s->width;
                rect->height = s->height;
                break;
            }
        }
        case 4: {
            rect->x = NUM2INT(arg1);
            rect->y = NUM2INT(arg2);
            rect->width = NUM2INT(arg3);
            rect->height = NUM2INT(arg4);
            break;
        }
    }
    return Qnil;
}

static VALUE rpg_rect_from_ltrb(VALUE klass, VALUE left, VALUE right, VALUE top, VALUE bottom) {
    RPGrect *rect = ALLOC(RPGrect);
    rect->x = NUM2INT(left);
    rect->y = NUM2INT(top);
    rect->width = NUM2INT(right) - rect->x;
    rect->height = NUM2INT(bottom) - rect->y;
    return Data_Wrap_Struct(klass, NULL, RUBY_DEFAULT_FREE, rect);
}

static VALUE rpg_point_set(VALUE self, VALUE x, VALUE y) {
    RPGpoint *point = DATA_PTR(self);
    point->x = NUM2INT(x);
    point->y = NUM2INT(y);
    return self;
}

static VALUE rpg_size_set(VALUE self, VALUE width, VALUE height) {
    RPGsize *size = DATA_PTR(self);
    size->width = NUM2INT(width);
    size->height = NUM2INT(height);
    return self;
}

static VALUE rpg_rect_set(VALUE self, VALUE x, VALUE y, VALUE width, VALUE height) {
    RPGrect *rect = DATA_PTR(self);
    rect->x = NUM2INT(x);
    rect->y = NUM2INT(y);
    rect->width = NUM2INT(width);
    rect->height = NUM2INT(height);
    return self;
}

static VALUE rpg_rect_right(VALUE self) {
    RPGrect *rect = DATA_PTR(self);
    return INT2NUM(rect->x + rect->width);
}

static VALUE rpg_rect_bottom(VALUE self) {
    RPGrect *rect = DATA_PTR(self);
    return INT2NUM(rect->y + rect->height);
}

static VALUE rpg_point_equal(VALUE self, VALUE other) {
    if (CLASS_OF(self) != CLASS_OF(other)) {
        return Qfalse;
    }
    RPGpoint *v1 = DATA_PTR(self);
    RPGpoint *v2 = DATA_PTR(other);
    return RB_BOOL(v1->x == v2->x && v1->y == v2->y);
}

static VALUE rpg_size_equal(VALUE self, VALUE other) {
    if (CLASS_OF(self) != CLASS_OF(other)) {
        return Qfalse;
    }
    RPGsize *v1 = DATA_PTR(self);
    RPGsize *v2 = DATA_PTR(other);
    return RB_BOOL(v1->width == v2->width && v1->height == v2->height);  
}

static VALUE rpg_rect_equal(VALUE self, VALUE other) {
    if (CLASS_OF(self) != CLASS_OF(other)) {
        return Qfalse;
    }
    RPGrect *v1 = DATA_PTR(self);
    RPGrect *v2 = DATA_PTR(other);
    return RB_BOOL(v1->x == v2->x && v1->y == v2->y && v1->width == v2->width && v1->height == v2->height);  
}

static VALUE rpg_point_inspect(VALUE self) {
    RPGpoint *obj = DATA_PTR(self);
    return rb_sprintf("<Point: x:%d y:%d>", obj->x, obj->y);
}

static VALUE rpg_size_inspect(VALUE self) {
    RPGsize *obj = DATA_PTR(self);
    return rb_sprintf("<Size: width:%d height:%d>", obj->width, obj->height);
}

static VALUE rpg_rect_inspect(VALUE self) {
    RPGrect *obj = DATA_PTR(self);
    return rb_sprintf("<Rect: x:%d y:%d width:%d height:%d>", obj->x, obj->y, obj->width, obj->height);
}

static VALUE rpg_point_empty_p(VALUE self) {
    RPGpoint *obj = DATA_PTR(self);
    return RB_BOOL(obj->x == 0 && obj->y == 0);
}

static VALUE rpg_size_empty_p(VALUE self) {
    RPGsize *obj = DATA_PTR(self);
    return RB_BOOL(obj->width == 0 && obj->height == 0);
}

static VALUE rpg_rect_empty_p(VALUE self) {
    RPGrect *obj = DATA_PTR(self);
    return RB_BOOL(obj->x == 0 && obj->y == 0 && obj->width == 0 && obj->height == 0);
}

static VALUE rpg_rect_area(VALUE self) {
    RPGrect *rect= DATA_PTR(self);
    return INT2NUM(rect->width * rect->height);
}

static VALUE rpg_rect_perimeter(VALUE self) {
    RPGrect *rect= DATA_PTR(self);
    return INT2NUM((rect->width + rect->height) * 2);
}

static VALUE rpg_rect_location(VALUE self) {
    RPGrect *rect= DATA_PTR(self);
    RPGpoint *point = ALLOC(RPGpoint);
    memcpy(point, rect, sizeof(RPGpoint));
    return Data_Wrap_Struct(rb_cPoint, NULL, RUBY_DEFAULT_FREE, point);
}

static VALUE rpg_rect_size(VALUE self) {
    RPGrect *rect= DATA_PTR(self);
    RPGsize *size = ALLOC(RPGsize);
    size->width = rect->width;
    size->height = rect->height;
    return Data_Wrap_Struct(rb_cSize, NULL, RUBY_DEFAULT_FREE, size);
}

static VALUE rpg_rect_include_p(int argc, VALUE *argv, VALUE self) {
    VALUE arg1, arg2;
    rb_scan_args(argc, argv, "11", &arg1, &arg2);
    RPGrect *rect= DATA_PTR(self);

    if (argc == 1) {
        if (rb_obj_is_kind_of(CLASS_OF(arg1), rb_cRect)) {
            RPGrect *other = DATA_PTR(arg1);
            return RB_BOOL(rect->x <= other->x) &&
                ((other->x + other->width) <= (rect->x + rect->width)) &&
                (rect->y <= other->y) &&
                ((other->y + other->height) <= (rect->y + rect->height));
        }
        RPGpoint *point = DATA_PTR(arg1);
        return RB_BOOL(rect->x <= point->x && point->x < rect->x + rect->width && rect->y <= point->y && point->y < rect->y + rect->height);
    }
    else {
        int x = NUM2INT(arg1);
        int y = NUM2INT(arg2);
        return RB_BOOL(rect->x <= x && x < rect->x + rect->width && rect->y <= y && y < rect->y + rect->height);
    }
}

static VALUE rpg_rect_inflate(int argc, VALUE *argv, VALUE self) {
    VALUE clone = rpg_rect_dup(self);
    return rpg_rect_inflate_bang(argc, argv, clone);
}

static VALUE rpg_rect_inflate_bang(int argc, VALUE *argv, VALUE self) {
    VALUE arg1, arg2;
    rb_scan_args(argc, argv, "11", &arg1, &arg2);

    RPGrect *rect = DATA_PTR(self);
    int width, height;
    if (argc == 1) {
        RPGsize *size = DATA_PTR(arg1);
        width = size->width;
        height = size->height;
    }
    else {
        width = NUM2INT(arg1);
        height = NUM2INT(arg2);
    }

    rect->x -= width;
    rect->y -= height;
    rect->width += 2 * width;
    rect->height += 2 * height;

    return self;
}

static VALUE rpg_rect_intersect(VALUE self, VALUE other) {
    VALUE clone = rpg_rect_dup(self);
    return rpg_rect_intersect_bang(clone, other);
}

static VALUE rpg_rect_intersect_bang(VALUE self, VALUE other) {

    RPGrect *a = DATA_PTR(self);
    RPGrect *b = DATA_PTR(other);

    int x1 = imax(a->x, b->x);
    int x2 = imin(a->x + a->width, b->x + b->width);
    int y1 = imax(a->y, b->y);
    int y2 = imin(a->y + a->height, b->y + b->height);

    if (x2 >= x1 && y2 >= y1) {
        a->x = x1;
        a->y = y1;
        a->width = x2 - x1;
        a->height = y2 - y1;
    }
    else {
        a->x = 0;
        a->y = 0;
        a->width = 0;
        a->height = 0;
    }
    return self;
}

static VALUE rpg_rect_intersects_p(VALUE self, VALUE other) {
    RPGrect *a = DATA_PTR(self);
    RPGrect *b = DATA_PTR(other);
    return RB_BOOL((b->x < a->x + a->width) && (a->x < (b->x + b->width)) && (b->y < a->y + a->height) && (a->y < b->y + b->height));
}

static VALUE rpg_rect_union(VALUE self, VALUE other) {
    VALUE clone = rpg_rect_dup(self);
    return rpg_rect_union_bang(clone, other);
}

static VALUE rpg_rect_union_bang(VALUE self, VALUE other) {
    RPGrect *a = DATA_PTR(self);
    RPGrect *b = DATA_PTR(other); 

    int x1 = imin(a->x, b->x);
    int x2 = imax(a->x + a->width, b->x + b->width);
    int y1 = imin(a->y, b->y);
    int y2 = imax(a->y + a->height, b->y + b->height);

    a->x = x1;
    a->y = y1;
    a->width = x2 - x1;
    a->height = y2 - y1;

    return self;
}

static VALUE rpg_rect_offset(int argc, VALUE *argv, VALUE self) {
    VALUE clone = rpg_rect_dup(self);
    return rpg_rect_offset_bang(argc, argv, clone);
}

static VALUE rpg_rect_offset_bang(int argc, VALUE *argv, VALUE self) {
    VALUE arg1, arg2;
    rb_scan_args(argc, argv, "11", &arg1, &arg2);

    RPGrect *rect = DATA_PTR(self);
    if (argc == 1) {
        RPGpoint *point = DATA_PTR(arg1);
        rect->x += point->x;
        rect->y += point->y;
    }
    else {
        rect->x += NUM2INT(arg1);
        rect->y += NUM2INT(arg2);
    }
    return self;
}

static VALUE rpg_point_add(VALUE self, VALUE other) {
    RPGpoint *a = DATA_PTR(self);
    RPGpoint *b = DATA_PTR(other);
    RPGpoint *result = ALLOC(RPGpoint);
    result->x = a->x + b->x;
    result->y = a->y + b->y;
    return Data_Wrap_Struct(CLASS_OF(self), NULL, RUBY_DEFAULT_FREE, result);
}

static VALUE rpg_point_minus(VALUE self, VALUE other) {
    RPGpoint *a = DATA_PTR(self);
    RPGpoint *b = DATA_PTR(other);
    RPGpoint *result = ALLOC(RPGpoint);
    result->x = a->x - b->x;
    result->y = a->y - b->y;
    return Data_Wrap_Struct(CLASS_OF(self), NULL, RUBY_DEFAULT_FREE, result);
}

static VALUE rpg_size_add(VALUE self, VALUE other) {
    RPGsize *a = DATA_PTR(self);
    RPGsize *b = DATA_PTR(other);
    RPGsize *result = ALLOC(RPGsize);
    result->width = a->width + b->width;
    result->height = a->height + b->height;
    return Data_Wrap_Struct(CLASS_OF(self), NULL, RUBY_DEFAULT_FREE, result);
}

static VALUE rpg_size_minus(VALUE self, VALUE other) {
    RPGsize *a = DATA_PTR(self);
    RPGsize *b = DATA_PTR(other);
    RPGsize *result = ALLOC(RPGsize);
    result->width = a->width - b->width;
    result->height = a->height - b->height;
    return Data_Wrap_Struct(CLASS_OF(self), NULL, RUBY_DEFAULT_FREE, result);
}

static VALUE rpg_point_offset(int argc, VALUE *argv, VALUE self) {
    VALUE clone = rpg_point_dup(self);
    return rpg_point_offset_bang(argc, argv, clone);
}

static VALUE rpg_point_offset_bang(int argc, VALUE *argv, VALUE self) {
    VALUE arg1, arg2;
    rb_scan_args(argc, argv, "11", &arg1, &arg2);

    RPGpoint *point = DATA_PTR(self);
    if (argc == 1) {
        RPGpoint *other = DATA_PTR(arg1);
        point->x += other->x;
        point->y += other->y;
    }
    else {
        point->x += NUM2INT(arg1);
        point->y += NUM2INT(arg2);
    }
    return self;
}