#include "list.h"
#include "main.h"

#include <stdint.h>
#include <stdlib.h>

static inline int
rand_bit(void)
{
    int r;

#if RAND_MAX % 2 == 1

    while ((r = rand()) != RAND_MAX) { }

#else

    r = rand();

#endif

    ASSUME(r >= 0);

    return r % 2;
}

struct list {
    uintptr_t *ptr;
    size_t count;
    size_t bytes;
}

void
list_init(struct list *list)
{
    if (IS_NULLPTR(list)) {
        return;
    }

    list->ptr = NULL;
    list->count = 0;
    list->bytes = 0;
}

void
list_add(struct list *list, void *ptr)
{
    if (IS_NULLPTR(list)) {
        return;
    }

    list_reserve(list, list->count + 1);

    list->ptr[list->count++] = (uintptr_t)ptr;
}
