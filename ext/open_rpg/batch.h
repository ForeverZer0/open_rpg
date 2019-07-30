#ifndef OPEN_RPG_BATCH_H
#define OPEN_RPG_BATCH_H 1

#include "common.h"

#define BATCH_INIT_CAPACITY 4

// #define RPG_BATCH_INIT(vec) RPGbatch vec; rpg_batch_init(&vec)
// #define RPG_BATCH_ADD(vec, item) rpg_batch_add(&vec, (void *) item)
// #define RPG_BATCH_SET(vec, id, item) rpg_batch_set(&vec, id, (void *) item)
// #define RPG_BATCH_GET(vec, type, id) (type) rpg_batch_get(&vec, id)
// #define RPG_BATCH_DELETE(vec, id) rpg_batch_delete(&vec, id)
// #define RPG_BATCH_TOTAL(vec) rpg_batch_total(&vec)
// #define RPG_BATCH_FREE(vec) rpg_batch_free(&vec)
// #define RPG_BATCH_SORT(vec) rpg_batch_sort(&vec, 0, vec->total - 1)


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