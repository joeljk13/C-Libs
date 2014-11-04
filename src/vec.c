#include "main.h"
#include "vec.h"

#include "alloc.h"

int
vec_reserve_one(void *ptr, size_t n, size_t size)
{
    void *tmp;
    size_t bytes;

    ASSUME(ptr != NULL);

    bytes = (n + 1) * size;

    tmp = jrealloc(*(void **)ptr, bytes);
    if (ERR(tmp == NULL)) {
        return -1;
    }
    *(void **)ptr = tmp;

    return 0;
}

int
vec_reserve_one_min(void *ptr, size_t *n, size_t size)
{
    void *tmp;
    size_t bytes;
    size_t cap;

    ASSUME(ptr != NULL);
    ASSUME(n != NULL);

    cap = (*n * 2);

    tmp = jrealloc(*(void **)ptr, cap * size);
    if (ERR(tmp == NULL)) {
        cap = (*n + 1);

        tmp = jrealloc(*(void **)ptr, cap * size);
        if (ERR(tmp == NULL)) {
            return -1;
        }
    }
    *(void **)ptr = tmp;

    *n = cap;

    return 0;
}

int
vec_reserve(void *ptr, size_t n, size_t size, size_t extra)
{
    void *tmp;

    ASSUME(ptr != NULL);

    tmp = jrealloc(*(void **)ptr, (n + extra) * size);
    if (ERR(tmp == NULL)) {
        return -1;
    }
    *(void **)ptr = tmp;

    return 0;
}

int
vec_reserve_min(void *ptr, size_t *n, size_t size, size_t extra)
{
    void *tmp;
    size_t cap;

    ASSUME(ptr != NULL);
    ASSUME(n != NULL);

    cap = extra < *n ? *n * 2 : *n + extra;

    tmp = jrealloc(*(void **)ptr, cap * size);
    if (ERR(tmp == NULL)) {
        cap = *n + extra;

        tmp = jrealloc(*(void **)ptr, cap * size);
        if (ERR(tmp == NULL)) {
            return -1;
        }
    }
    *(void **)ptr = tmp;

    *n = cap;

    return 0;
}

int
vec_shrink(void *ptr, size_t *n, size_t size, size_t m)
{
    void *tmp;

    tmp = jrealloc(*(void **)ptr, m * size);
    if (ERR(tmp == NULL)) {
        return -1;
    }
    *(void **)ptr = tmp;

    *n = m;

    return 0;
}
