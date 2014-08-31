#include "data.h"
#include "main.h"

#include <stdint.h>
#include <stdlib.h>

struct data {
    uintptr_t *ptr;
    size_t count;
    size_t bytes;
}

void
data_init(struct data *data)
{
    if (IS_NULLPTR(data)) {
        return;
    }

    data->ptr = NULL;
    data->count = 0;
    data->bytes = 0;
}

void
data_add(struct data *data, void *ptr)
{
    if (IS_NULLPTR(data)) {
        return;
    }

    data_reserve(data, data->count + 1);

    data->ptr[data->count++] = (uintptr_t)ptr;
}

void *
data_find(struct data *data, void *ptr
