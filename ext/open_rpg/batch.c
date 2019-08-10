#include "./batch.h"

void rpg_batch_init(RPGbatch *v) {
    v->capacity = BATCH_INIT_CAPACITY;
    v->total = 0;
    v->items = xmalloc(sizeof(void *) * BATCH_INIT_CAPACITY);
}

int rpg_batch_total(RPGbatch *v) { return v->total; }

static void rpg_batch_resize(RPGbatch *v, int capacity) {
    RPGrenderable **items = xrealloc(v->items, sizeof(void *) * capacity);
    if (items) {
        v->items = items;
        v->capacity = capacity;
    }
}

void rpg_batch_add(RPGbatch *v, RPGrenderable *item) {
    if (v->capacity == v->total) {
        rpg_batch_resize(v, v->capacity * 2);
    }
    v->items[v->total++] = item;
    v->updated = GL_TRUE;
}

void rpg_batch_set(RPGbatch *v, int index, RPGrenderable *item) {
    if (index >= 0 && index < v->total) {
        v->items[index] = item;
        v->updated = GL_TRUE;
    }
}

RPGrenderable *rpg_batch_get(RPGbatch *v, int index) {
    if (index >= 0 && index < v->total) {
        return v->items[index];
    }
    return NULL;
}

void rpg_batch_delete_item(RPGbatch *batch, RPGrenderable *item) {
    if (item == NULL) {
        return;
    }
    for (int i = 0; i < batch->total; i++) {
        if (batch->items[i] == item) {
            rpg_batch_delete(batch, i);
            break;
        }
    }
}

void rpg_batch_delete(RPGbatch *v, int index) {
    if (index < 0 || index >= v->total) {
        return;
    }

    v->items[index] = NULL;

    for (int i = index; i < v->total - 1; i++) {
        v->items[i] = v->items[i + 1];
        v->items[i + 1] = NULL;
    }

    v->total--;

    if (v->total > 0 && v->total == v->capacity / 4) {
        rpg_batch_resize(v, v->capacity / 2);
    }
}

static inline int rpg_batch_median_of_3(int a, int b, int c) {
    return imax(imin(a,b), imin(imax(a,b),c));
}

void rpg_batch_sort(RPGbatch *v, int first, int last) {
    int i, j, pivot;
    RPGrenderable *temp;
    if (first < last) {
        pivot = ((last - first) / 2) + first;
        i = first;
        j = last;
        while (i < j) {
            while (v->items[i]->z <= v->items[pivot]->z && i < last) {
                i++;
            }
            while (v->items[j]->z > v->items[pivot]->z) {
                j--;
            }
            if (i < j) {
                temp = v->items[i];
                v->items[i] = v->items[j];
                v->items[j] = temp;
            }
        }

        temp = v->items[pivot];
        v->items[pivot] = v->items[j];
        v->items[j] = temp;
        rpg_batch_sort(v, first, j - 1);
        rpg_batch_sort(v, j + 1, last);
    }
    v->updated = GL_FALSE;
}