#ifndef OPEN_RPG_RENDER_BATCH_H
#define OPEN_RPG_RENDER_BATCH_H 1

#include "common.h"

void rpg_batch_init(VALUE parent);
void rpg_batch_render(RPGbatch *group);

void rpg_batch_add(RPGbatch *batch, void *sprite);
void rpg_batch_remove(RPGbatch *batch, void *sprite);

static VALUE rpg_batch_alloc(VALUE klass);

typedef struct RPGz {
    GLint z;
} RPGz;

#endif /* OPEN_RPG_RENDER_BATCH_H */