
#include "./internal.h"

VALUE rb_cTimeSpan;

#define MS_IN_SEC 1000
#define MS_IN_MIN 60000
#define MS_IN_HOUR 3600000
#define MS_IN_DAY 86400000
#define MS_IN_YEAR 31536000000

ALLOC_FUNC(rpg_span_alloc, RPGtimespan)

static void rpg_span_calculate(GLuint64 value, GLuint64 *ms, GLuint64 *sec, GLuint64 *min, GLuint64 *hr, GLuint64 *day, GLuint64 *yr) {
    GLuint64 quotient;

    quotient = value / MS_IN_YEAR;
    value %= MS_IN_YEAR;
    if (yr != NULL)
        *yr = quotient;

    quotient = value / MS_IN_DAY;
    value %= MS_IN_DAY;
    if (day != NULL)
        *day = quotient;

    quotient = value / MS_IN_HOUR;
    value %= MS_IN_HOUR;
    if (hr != NULL)
        *hr = quotient;

    quotient = value / MS_IN_MIN;
    value %= MS_IN_MIN;
    if (min != NULL) 
        *min = quotient;

    quotient = value / MS_IN_SEC;
    value %= MS_IN_SEC;
    if (sec != NULL)
        *sec = quotient;

    if (ms != NULL)
        *ms = value;
}

static VALUE rpg_span_frames(VALUE self) {
    RPGtimespan *ts = DATA_PTR(self);
    return DBL2NUM((ts->ms / (GLdouble) MS_IN_SEC) / rpgTICK);
}

static VALUE rpg_span_total_ms(VALUE self) {
    RPGtimespan *ts = DATA_PTR(self);
    return DBL2NUM((GLdouble) ts->ms);
}

static VALUE rpg_span_total_sec(VALUE self) {
    RPGtimespan *ts = DATA_PTR(self);
    return DBL2NUM(ts->ms / (GLdouble) MS_IN_SEC);
}

static VALUE rpg_span_total_min(VALUE self) {
    RPGtimespan *ts = DATA_PTR(self);
    return DBL2NUM(ts->ms / (GLdouble) MS_IN_MIN);
}

static VALUE rpg_span_total_hr(VALUE self) {
    RPGtimespan *ts = DATA_PTR(self);
    return DBL2NUM(ts->ms / (GLdouble) MS_IN_HOUR);
}

static VALUE rpg_span_total_day(VALUE self) {
    RPGtimespan *ts = DATA_PTR(self);
    return DBL2NUM(ts->ms / (GLdouble) MS_IN_DAY);
}

static VALUE rpg_span_total_yr(VALUE self) {
    RPGtimespan *ts = DATA_PTR(self);
    return DBL2NUM(ts->ms / (GLdouble) MS_IN_YEAR);
}

static VALUE rpg_span_ms(VALUE self) {
    RPGtimespan *ts = DATA_PTR(self);
    GLuint64 v;
    rpg_span_calculate(ts->ms, &v, NULL, NULL, NULL, NULL, NULL);
    return ULL2NUM(v);
}

static VALUE rpg_span_sec(VALUE self) {
    RPGtimespan *ts = DATA_PTR(self);
    GLuint64 v;
    rpg_span_calculate(ts->ms, NULL, &v, NULL, NULL, NULL, NULL);
    return ULL2NUM(v);
}

static VALUE rpg_span_min(VALUE self) {
    RPGtimespan *ts = DATA_PTR(self);
    GLuint64 v;
    rpg_span_calculate(ts->ms, NULL, NULL, &v, NULL, NULL, NULL);
    return ULL2NUM(v);
}

static VALUE rpg_span_hr(VALUE self) {
    RPGtimespan *ts = DATA_PTR(self);
    GLuint64 v;
    rpg_span_calculate(ts->ms, NULL, NULL, NULL, &v, NULL, NULL);
    return ULL2NUM(v);
}

static VALUE rpg_span_day(VALUE self) {
    RPGtimespan *ts = DATA_PTR(self);
    GLuint64 v;
    rpg_span_calculate(ts->ms, NULL, NULL, NULL, NULL, &v, NULL);
    return ULL2NUM(v);
}

static VALUE rpg_span_yr(VALUE self) {
    RPGtimespan *ts = DATA_PTR(self);
    GLuint64 v;
    rpg_span_calculate(ts->ms, NULL, NULL, NULL, NULL, NULL, &v);
    return ULL2NUM(v);
}

static VALUE rpg_span_from_frames(VALUE klass, VALUE value) {
    RPGtimespan *ts = ALLOC(RPGtimespan);
    ts->ms = (GLuint64) round(NUM2UINT(value) * rpgTICK * MS_IN_SEC);
    return Data_Wrap_Struct(klass, NULL, RUBY_DEFAULT_FREE, ts);
}

static VALUE rpg_span_from_ms(VALUE klass, VALUE value) {
    RPGtimespan *ts = ALLOC(RPGtimespan);
    ts->ms = NUM2ULL(value);
    return Data_Wrap_Struct(klass, NULL, RUBY_DEFAULT_FREE, ts);
}

static VALUE rpg_span_from_sec(VALUE klass, VALUE value) {
    RPGtimespan *ts = ALLOC(RPGtimespan);
    ts->ms = NUM2ULL(value) * MS_IN_SEC;
    return Data_Wrap_Struct(klass, NULL, RUBY_DEFAULT_FREE, ts);
}

static VALUE rpg_span_from_min(VALUE klass, VALUE value) {
    RPGtimespan *ts = ALLOC(RPGtimespan);
    ts->ms = NUM2ULL(value) * MS_IN_MIN;
    return Data_Wrap_Struct(klass, NULL, RUBY_DEFAULT_FREE, ts);
}

static VALUE rpg_span_from_hr(VALUE klass, VALUE value) {
    RPGtimespan *ts = ALLOC(RPGtimespan);
    ts->ms = NUM2ULL(value) * MS_IN_HOUR;
    return Data_Wrap_Struct(klass, NULL, RUBY_DEFAULT_FREE, ts);
}

static VALUE rpg_span_from_day(VALUE klass, VALUE value) {
    RPGtimespan *ts = ALLOC(RPGtimespan);
    ts->ms = NUM2ULL(value) * MS_IN_DAY;
    return Data_Wrap_Struct(klass, NULL, RUBY_DEFAULT_FREE, ts);
}

static VALUE rpg_span_from_yr(VALUE klass, VALUE value) {
    RPGtimespan *ts = ALLOC(RPGtimespan);
    ts->ms = NUM2ULL(value) * MS_IN_YEAR;
    return Data_Wrap_Struct(klass, NULL, RUBY_DEFAULT_FREE, ts);
}

static VALUE rpg_span_initialize(int argc, VALUE *argv, VALUE self) {
    VALUE ms, sec, min, hr, day, yr;
    rb_scan_args(argc, argv, "15", &ms, &sec, &min, &hr, &day, &yr);
    RPGtimespan *ts = DATA_PTR(self);
    ts->ms = NUM2ULL(ms);
    if (argc > 1) {
        ts->ms += NUM2ULL(sec) * MS_IN_SEC;
        if (argc > 2) {
            ts->ms += NUM2ULL(min) * MS_IN_MIN;
            if (argc > 3) {
                ts->ms += NUM2ULL(hr) * MS_IN_HOUR;
                if (argc > 4) {
                    ts->ms += NUM2ULL(day) * MS_IN_DAY;
                    if (argc > 5) {
                        ts->ms += NUM2ULL(yr) * MS_IN_YEAR;
                    }
                }
            }
        }
    }
    return Qnil;
}

static VALUE rpg_span_equal(VALUE self, VALUE other) {
    if (CLASS_OF(self) != CLASS_OF(other)) {
        return Qfalse;
    }
    RPGtimespan *s1 = DATA_PTR(self), *s2 = DATA_PTR(other);
    return RB_BOOL(s1->ms == s2->ms);
}

static VALUE rpg_span_add(VALUE self, VALUE other) {
    RPGtimespan *s1 = DATA_PTR(self), *s2 = DATA_PTR(other), *ts = ALLOC(RPGtimespan);
    ts->ms = s1->ms + s2->ms;
    return Data_Wrap_Struct(CLASS_OF(self), NULL, RUBY_DEFAULT_FREE, ts);
}

static VALUE rpg_span_subtract(VALUE self, VALUE other) {
    RPGtimespan *s1 = DATA_PTR(self), *s2 = DATA_PTR(other), *ts = ALLOC(RPGtimespan);
    if (s1->ms > s2->ms) {
        ts->ms = s1->ms - s2->ms;
    } else {
        ts->ms = s2->ms - s1->ms;
    }
    return Data_Wrap_Struct(CLASS_OF(self), NULL, RUBY_DEFAULT_FREE, ts);
}

static VALUE rpg_span_multiply(VALUE self, VALUE other) {
    RPGtimespan *s1 = DATA_PTR(self), *ts = ALLOC(RPGtimespan);
    ts->ms = (GLuint64) round(s1->ms * NUM2DBL(other));
    return Data_Wrap_Struct(CLASS_OF(self), NULL, RUBY_DEFAULT_FREE, ts);
}

static VALUE rpg_span_divide(VALUE self, VALUE other) {
    RPGtimespan *s1 = DATA_PTR(self), *ts = ALLOC(RPGtimespan);
    GLdouble div = NUM2DBL(other);
    if (div == 0.0) {
        rb_raise(rb_eZeroDivError, "only CHuck Norris can divide by zero");
    }
    ts->ms = (GLuint64) round(s1->ms / div);
    return Data_Wrap_Struct(CLASS_OF(self), NULL, RUBY_DEFAULT_FREE, ts);
}

static VALUE rpg_span_compare(VALUE self, VALUE other) {
    if (RB_IS_A(other, rb_cTimeSpan)) {
        RPGtimespan *s1 = DATA_PTR(self), *s2 = DATA_PTR(other);
        if (s1->ms < s2->ms)
            return INT2NUM(-1);
        if (s1->ms > s2->ms)
            return INT2NUM(1);
        return INT2NUM(0);
    }
    return Qnil;
}

static VALUE rpg_span_dump(int argc, VALUE *argv, VALUE self) {
    RPGtimespan *ts = DATA_PTR(self);
    return rb_str_new((void*) ts, sizeof(RPGtimespan));
}

static VALUE rpg_span_load(VALUE klass, VALUE buffer) {
    void *data = StringValuePtr(buffer);
    RPGtimespan *ts = ALLOC(RPGtimespan);
    memcpy(ts, data, sizeof(RPGtimespan));
    return Data_Wrap_Struct(klass, NULL, RUBY_DEFAULT_FREE, ts);
}

static VALUE rpg_span_dup(VALUE self) {
    RPGtimespan *s1 = DATA_PTR(self), *ts = ALLOC(RPGtimespan);
    memcpy(ts, s1, sizeof(RPGtimespan));
    return Data_Wrap_Struct(CLASS_OF(self), NULL, RUBY_DEFAULT_FREE, ts);
}

static VALUE rpg_span_to_h(VALUE self) {
    RPGtimespan *ts = DATA_PTR(self);
    GLuint64 ms, sec, min, hr, day, yr;
    rpg_span_calculate(ts->ms, &ms, &sec, &min, &hr, &day, &yr);

    VALUE hash = rb_hash_new();
    rb_hash_aset(hash, STR2SYM("milliseconds"), ULL2NUM(ms));
    rb_hash_aset(hash, STR2SYM("seconds"), ULL2NUM(sec));
    rb_hash_aset(hash, STR2SYM("minutes"), ULL2NUM(min));
    rb_hash_aset(hash, STR2SYM("hours"), ULL2NUM(hr));
    rb_hash_aset(hash, STR2SYM("days"), ULL2NUM(day));
    rb_hash_aset(hash, STR2SYM("years"), ULL2NUM(yr));
    return hash;
}

static VALUE rpg_span_to_s(VALUE self) {
    RPGtimespan *ts = DATA_PTR(self);
    if (ts->ms == 0) {
        return rb_sprintf("0 ms");
    }
    VALUE ary = rb_ary_new_capa(6);
    GLuint64 ms, sec, min, hr, day, yr;
    rpg_span_calculate(ts->ms, &ms, &sec, &min, &hr, &day, &yr);

    if (yr > 0) {
        rb_ary_push(ary, rb_sprintf("%u %s", yr, yr > 0 ? "years" : "year"));
    } 

    if (day > 0) {
        rb_ary_push(ary, rb_sprintf("%u %s", day, day > 0 ? "days" : "day"));
    } 

    if (hr > 0) {
        rb_ary_push(ary, rb_sprintf("%u %s", hr, hr > 0 ? "hours" : "hour"));
    } 

    if (min > 0) {
        rb_ary_push(ary, rb_sprintf("%u %s", min, min > 0 ? "minutes" : "minute"));
    } 

    if (sec > 0) {
        rb_ary_push(ary, rb_sprintf("%u %s", sec, sec > 0 ? "seconds" : "second"));
    } 

    if (ms > 0) {
        rb_ary_push(ary, rb_sprintf("%u %s", ms, ms > 0 ? "milliseconds" : "millisecond"));
    } 

    return rb_ary_join(ary, rb_str_new_cstr(", "));
}

static VALUE rpg_span_inspect(VALUE self) {
    RPGtimespan *ts = DATA_PTR(self);
    GLuint64 ms, sec, min, hr, day, yr;
    rpg_span_calculate(ts->ms, &ms, &sec, &min, &hr, &day, &yr);
    return rb_sprintf("<TimeSpan: yr:%u day:%u hr:%u min:%u sec:%u ms:%u>", yr, day, hr, min, sec, ms);
}

void rpg_timespan_init(VALUE parent) {
    rb_cTimeSpan = rb_define_class_under(parent, "TimeSpan", rb_cObject);
    rb_define_alloc_func(rb_cTimeSpan, rpg_span_alloc);
    rb_include_module(rb_cTimeSpan, rb_mComparable);

    rb_define_method(rb_cTimeSpan, "initialize", rpg_span_initialize, -1);

    rb_define_method(rb_cTimeSpan, "frames", rpg_span_frames, 0);
    rb_define_method(rb_cTimeSpan, "milliseconds", rpg_span_ms, 0);
    rb_define_method(rb_cTimeSpan, "seconds", rpg_span_sec, 0);
    rb_define_method(rb_cTimeSpan, "minutes", rpg_span_min, 0);
    rb_define_method(rb_cTimeSpan, "hours", rpg_span_hr, 0);
    rb_define_method(rb_cTimeSpan, "days", rpg_span_day, 0);
    rb_define_method(rb_cTimeSpan, "years", rpg_span_yr, 0);

    rb_define_method(rb_cTimeSpan, "total_milliseconds", rpg_span_total_ms, 0);
    rb_define_method(rb_cTimeSpan, "total_seconds", rpg_span_total_sec, 0);
    rb_define_method(rb_cTimeSpan, "total_minutes", rpg_span_total_min, 0);
    rb_define_method(rb_cTimeSpan, "total_hours", rpg_span_total_hr, 0);
    rb_define_method(rb_cTimeSpan, "total_days", rpg_span_total_day, 0);
    rb_define_method(rb_cTimeSpan, "total_years", rpg_span_total_yr, 0);

    rb_define_singleton_method(rb_cTimeSpan, "from_frames", rpg_span_from_frames, 1);
    rb_define_singleton_method(rb_cTimeSpan, "from_milliseconds", rpg_span_from_ms, 1);
    rb_define_singleton_method(rb_cTimeSpan, "from_seconds", rpg_span_from_sec, 1);
    rb_define_singleton_method(rb_cTimeSpan, "from_minutes", rpg_span_from_min, 1);
    rb_define_singleton_method(rb_cTimeSpan, "from_hours", rpg_span_from_hr, 1);
    rb_define_singleton_method(rb_cTimeSpan, "from_days", rpg_span_from_day, 1);
    rb_define_singleton_method(rb_cTimeSpan, "from_years", rpg_span_from_yr, 1);

    rb_define_method(rb_cTimeSpan, "==", rpg_span_equal, 1);
    rb_define_method(rb_cTimeSpan, "<=>", rpg_span_compare, 1);
    rb_define_method(rb_cTimeSpan, "+", rpg_span_add, 1);
    rb_define_method(rb_cTimeSpan, "-", rpg_span_subtract, 1);
    rb_define_method(rb_cTimeSpan, "*", rpg_span_multiply, 1);
    rb_define_method(rb_cTimeSpan, "/", rpg_span_divide, 1);
    rb_define_method(rb_cTimeSpan, "to_s", rpg_span_to_s, 0);
    rb_define_method(rb_cTimeSpan, "inspect", rpg_span_inspect, 0);
    rb_define_method(rb_cTimeSpan, "dup", rpg_span_dup, 0);
    rb_define_method(rb_cTimeSpan, "to_h", rpg_span_to_h, 0);

    rb_define_alias(rb_cTimeSpan, "add", "+");
    rb_define_alias(rb_cTimeSpan, "subtract", "-");
    rb_define_alias(rb_cTimeSpan, "multiply", "*");
    rb_define_alias(rb_cTimeSpan, "divide", "/");
    rb_define_alias(rb_cTimeSpan, "ticks", "frames");

    rb_define_method(rb_cTimeSpan, "_dump", rpg_span_dump, -1);
    rb_define_singleton_method(rb_cTimeSpan, "_load", rpg_span_load, 1);

    RPGtimespan *span = ALLOC(RPGtimespan);
    span->ms = (GLuint64) 0xFFFFFFFFFFFFFFFF;
    rb_define_const(rb_cTimeSpan, "MAX", Data_Wrap_Struct(rb_cTimeSpan, NULL, RUBY_DEFAULT_FREE, span));
    rb_define_const(rb_cTimeSpan, "ZERO", rpg_span_alloc(rb_cTimeSpan));
}