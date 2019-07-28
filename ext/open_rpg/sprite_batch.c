#include "sprite_batch.h"

VALUE rb_cSpriteBatch;

void rpg_batch_init(VALUE parent)
{
    rb_cSpriteBatch = rb_define_class_under(parent, "SpriteBatch", rb_cObject);
    rb_define_alloc_func(rb_cSpriteBatch, rpg_batch_alloc);
}

ALLOC_FUNC(rpg_batch_alloc, RPGbatch)

void rpg_batch_render(RPGbatch *group)
{
}

inline void quicksort(RPGz **data, int first, int last)
{
    int i, j, pivot, temp;
    if (first < last)
    {
        pivot = first;
        i = first;
        j = last;
        while (i < j)
        {
            while (data[i]->z <= data[pivot]->z && i < last)
            {
                i++;
            }
            while (data[j]->z > data[pivot]->z)
            {
                j--;
            }
            if (i < j)
            {
                temp = data[i]->z;
                data[i]->z = data[j]->z;
                data[j]->z = temp;
            }
        }
        temp = data[pivot]->z;
        data[pivot]->z = data[j]->z;
        data[j]->z = temp;
        quicksort(data, first, j - 1);
        quicksort(data, j + 1, last);
    }
}

void rpg_batch_sort(RPGbatch *batch)
{

    int count = batch->count;
    for (int i = 0; i < count; i++)
    {
        if (batch->children[i] == NULL)
        {
            batch->count--;
        }
    }
    batch->updated = GL_FALSE;
}

void rpg_batch_add(RPGbatch *batch, void *sprite)
{
    if (batch->capacity == batch->count)
    {
        // TODO: realloc
    }
    batch->children[batch->count] = sprite;
    batch->updated = GL_TRUE;
}

void rpg_batch_remove(RPGbatch *batch, void *sprite)
{
    for (int i = 0; i < batch->count; i++)
    {
        if (batch->children[i] == sprite)
        {
            batch->children[i] = NULL;
            batch->updated = GL_TRUE;
        }
    }
}