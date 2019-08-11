#include "./transition.h"

typedef struct cached_shader {
    ID key;
    VALUE shader;
    UT_hash_handle hh;
} cached_shader;

cached_shader *cache;

VALUE rb_mTransition;

void rpg_transition_init(VALUE parent) {
    rb_mTransition = rb_define_module_under(parent, "Transition");

    rb_define_singleton_method(rb_mTransition, "circle", _circle, -1);

    cache = NULL;
}

static inline cached_shader *get_cached(ID id) {
    cached_shader *s = NULL;
    HASH_FIND(hh, cache, &id, sizeof(ID), s);
    return s;
}

static VALUE _circle(int argc, VALUE *argv, VALUE module) {
    ID id = rb_intern("circle");
    cached_shader *s = get_cached(id);
    if (s == NULL) {
        
    }

    return Qnil;
}