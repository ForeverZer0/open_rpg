#ifndef OPEN_RPG_TRANSITION_H
#define OPEN_RPG_TRANSITION_H 1

#include "./common.h"

typedef struct RPGtransition {
    RPGimage *from;
    RPGimage *to;
    VALUE program;
    GLint frames;
} RPGtransition;

void rpg_transition_init(VALUE parent);

static VALUE rpg_transition_execute(VALUE self, VALUE shader, VALUE frames);

#endif /* OPEN_RPG_TRANSITION_H */