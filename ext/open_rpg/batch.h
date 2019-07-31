#ifndef OPEN_RPG_BATCH_H
#define OPEN_RPG_BATCH_H 1

#include "./common.h"

#define BATCH_INIT_CAPACITY 4

void rpg_batch_init(RPGbatch *batch);
int rpg_batch_total(RPGbatch *batch);
static void rpg_batch_resize(RPGbatch *batch, int capacity);
void rpg_batch_add(RPGbatch *batch, RPGrenderable *item);
void rpg_batch_set(RPGbatch *batch, int index, RPGrenderable *item);
RPGrenderable *rpg_batch_get(RPGbatch *batch, int index);
void rpg_batch_delete(RPGbatch *bacth, int index);
void rpg_batch_free(RPGbatch *batch);
void rpg_batch_sort(RPGbatch *batch, int first, int last);

#endif /* OPEN_RPG_BATCH_H */