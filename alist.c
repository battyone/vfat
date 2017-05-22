#include <assert.h>

#include "alist.h"

#define INIT_CAPACITY 4

static void alist_realloc(struct alist *list, u32 new_capacity)
{
    void *new_items = malloc(list->item_size * new_capacity);
    memcpy(new_items, list->items, list->cnt);
    free(list->items);
    list->items = new_items;
}

void alist_create(struct alist* list, u32 item_size)
{
    list->cnt = 0;
    list->item_size = item_size;
    list->capacity = INIT_CAPACITY;
    list->items = malloc(item_size * list->capacity);
}

void alist_add(struct alist *list, void *item)
{
    if (list->cnt + 1 > list->capacity) {
        list->capacity <<= 1;
        alist_realloc(list, list->capacity);
    }

    memcpy(list->items + list->cnt, item, list->item_size);
    ++(list->cnt);
}

void alist_remove(struct alist *list, u32 idx)
{
    assert(idx < list->cnt);

    if ((list->cnt - 1) < (list->capacity >> 2)) {
        list->capacity >>= 1;
        void *new_items = malloc(list->item_size * list->capacity);
        // We can pass zero bytes in memcpy in C99
        memcpy(new_items, list->items, idx);
        memcpy(new_items + idx, list->items + idx + 1, list->cnt - idx - 1);
        free(list->items);
        list->items = new_items;
    } else {
        memmove(list->items + idx, list->items + idx + 1, list->cnt - idx - 1);
    }

    --(list->cnt);
}

void *alist_get(struct alist *list, u32 idx)
{
    //return &(list->items[idx]);
    return list + idx;
}

u32 alist_count(struct alist *list)
{
    return list->cnt;
}

void alist_free(struct alist *list)
{
    free(list->items);
}
