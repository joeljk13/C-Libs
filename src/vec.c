#include "main.h"
#include "vec.h"

#include "alloc.h"

int
vec_reserve_one(void **ptr, size_t n, size_t size)
{
    void *tmp;
    size_t bytes;

    ASSUME(ptr != NULL);

    bytes = (n + 1) * size;

    tmp = jrealloc(*ptr, bytes);
    if (ERR(tmp == NULL)) {
        return -1;
    }
    *ptr = tmp;

    return 0;
}

int
vec_reserve_one_min(void **ptr, size_t *n, size_t size)
{
    void *tmp;
    size_t bytes;
    size_t cap;

    ASSUME(ptr != NULL);
    ASSUME(n != NULL);

    cap = (*n * 2);

    tmp = jrealloc(*ptr, cap * size);
    if (ERR(tmp == NULL)) {
        cap = (*n + 1);

        tmp = jrealloc(*ptr, cap * size);
        if (ERR(tmp == NULL)) {
            return -1;
        }
    }
    *ptr = tmp;

    *n = cap;

    return 0;
}

int
vec_reserve(void **ptr, size_t n, size_t size, size_t extra)
{
    void *tmp;

    ASSUME(ptr != NULL);

    tmp = jrealloc(*ptr, (n + extra) * size);
    if (ERR(tmp == NULL)) {
        return -1;
    }
    *ptr = tmp;

    return 0;
}

int
vec_reserve_min(void **ptr, size_t *n, size_t size, size_t extra)
{
    void *tmp;
    size_t cap;

    ASSUME(ptr != NULL);
    ASSUME(n != NULL);

    cap = extra < *n ? *n * 2 : *n + extra;

    tmp = jrealloc(*ptr, cap * size);
    if (ERR(tmp == NULL)) {
        cap = *n + extra;

        tmp = jrealloc(*ptr, cap * size);
        if (ERR(tmp == NULL)) {
            return -1;
        }
    }
    *ptr = tmp;

    *n = cap;

    return 0;
}

int
vec_shrink(void **ptr, size_t *n, size_t size, size_t m)
{
    void *tmp;

    tmp = jrealloc(*ptr, m * size);
    if (ERR(tmp == NULL)) {
        return -1;
    }
    *ptr = tmp;

    *n = m;

    return 0;
}
