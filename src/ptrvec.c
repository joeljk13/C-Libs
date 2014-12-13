#include "main.h"
#include "ptrvec.h"

#include "alloc.h"
#include "vec.h"

#include <string.h>

int
ptrvec_init(struct ptrvec *ptrvec)
{
    ASSUME(ptrvec != NULL);

    ptrvec->ptr = NULL;
    ptrvec->length = 0;
    ptrvec->capacity = 0;

    return 0;
}

void
ptrvec_zero(struct ptrvec *ptrvec)
{
    ASSUME(ptrvec != NULL);

    // Can't use memset, because it's possible that NULL is not represented by
    // all 0 bits
    for (size_t i = 0; i < ptrvec->length; ++i) {
        ptrvec->ptr[i] = NULL;
    }
}

int
ptrvec_push(struct ptrvec *ptrvec, const void *ptr)
{
    ASSUME(ptrvec != NULL);

    if (ptrvec->capacity == ptrvec->length
        && ERR(vec_reserve_one_min(&ptrvec->ptr, &ptrvec->capacity,
                                   sizeof(*ptrvec->ptr)) != 0)) {

        return -1;
    }

    ptrvec->ptr[ptrvec->length++] = (void *)ptr;

    return 0;
}

int
ptrvec_push_v(struct ptrvec *ptrvec, struct ptrvec *ptr)
{
    ASSUME(ptrvec != NULL);
    ASSUME(ptr != NULL);

    if (ERR(ptrvec_reserve(ptrvec, ptrvec->length + ptr->length) != 0)) {
        return -1;
    }

    memcpy(ptrvec->ptr + ptrvec->length, ptr->ptr, ptr->length *
           sizeof(*ptr->ptr));

    ptrvec->length += ptr->length;

    return 0;
}

void *
ptrvec_pop(struct ptrvec *ptrvec)
{
    ASSUME(ptrvec != NULL);

    return ptrvec->ptr[--ptrvec->length];
}

void *
ptrvec_peek(struct ptrvec *ptrvec)
{
    ASSUME(ptrvec != NULL);

    return ptrvec->ptr[ptrvec->length - 1];
}

int
ptrvec_insert(struct ptrvec *ptrvec, const void *ptr, size_t index)
{
    ASSUME(ptrvec != NULL);
    ASSUME(index <= ptrvec->length);

    if (ptrvec->length == ptrvec->capacity
        && ERR(vec_reserve_one_min(&ptrvec->ptr, &ptrvec->capacity,
                                   sizeof(*ptrvec->ptr)) != 0)) {

        return -1;
    }

    memmove(ptrvec->ptr + index + 1, ptrvec->ptr + index,
            (ptrvec->length - index - 1) * sizeof(*ptrvec->ptr));

    ptrvec->ptr[index] = (void *)ptr;

    ++ptrvec->length;

    return 0;
}

int
ptrvec_insert_v(struct ptrvec *ptrvec, struct ptrvec *ptr, size_t index)
{
    ASSUME(ptrvec != NULL);
    ASSUME(ptr != NULL);
    ASSUME(index <= ptrvec->length);

    if (ERR(ptrvec_reserve(ptrvec, ptrvec->length + ptr->length) != 0)) {
        return -1;
    }

    memmove(ptrvec->ptr + index + ptr->length, ptrvec->ptr + index,
            ptr->length * sizeof(*ptr->ptr));
    // If it's inserting a ptrvec inside itself, it'll need a memmove
    memmove(ptrvec->ptr + index, ptr->ptr, ptr->length * sizeof(*ptr->ptr));

    ptrvec->length += ptr->length;

    return 0;
}

void
ptrvec_remove(struct ptrvec *ptrvec, size_t index)
{
    ASSUME(ptrvec != NULL);
    ASSUME(index < ptrvec->length);

    memmove(ptrvec->ptr + index, ptrvec->ptr + index + 1,
            (ptrvec->length - index - 1) * sizeof(*ptrvec->ptr));

    --ptrvec->length;
}

void
ptrvec_remove_r(struct ptrvec *ptrvec, size_t begin, size_t end)
{
    ASSUME(ptrvec != NULL);
    ASSUME(begin <= end);
    ASSUME(end <= ptrvec->length);

    memmove(ptrvec->ptr + begin, ptrvec->ptr + end, (ptrvec->length - end)
            * sizeof(*ptrvec->ptr));

    ptrvec->length -= (end - begin);
}

void
ptrvec_remove_fast(struct ptrvec *ptrvec, size_t index)
{
    ASSUME(ptrvec != NULL);
    ASSUME(index <= ptrvec->length);

    ptrvec->ptr[index] = ptrvec->ptr[--ptrvec->length];
}

void
ptrvec_remove_fast_r(struct ptrvec *ptrvec, size_t begin, size_t end)
{
    ASSUME(ptrvec != NULL);
    ASSUME(begin <= end);
    ASSUME(end <= ptrvec->length);

    memmove(ptrvec->ptr + begin, ptrvec->ptr + ptrvec->length - (end - begin),
            (end - begin) * sizeof(*ptrvec->ptr));

    ptrvec->length -= (end - begin);
}

int
ptrvec_contains(struct ptrvec *ptrvec, const void *ptr)
{
    ASSUME(ptrvec != NULL);

    for (size_t i = 0; i < ptrvec->length; ++i) {
        if (ptrvec->ptr[i] == ptr) {
            return 1;
        }
    }

    return 0;
}

size_t
ptrvec_find(struct ptrvec *ptrvec, const void *ptr)
{
    ASSUME(ptrvec != NULL);

    for (size_t i = 0; i < ptrvec->length; ++i) {
        if (ptrvec->ptr[i] == ptr) {
            return i;
        }
    }

    return ptrvec->length;
}

int
ptrvec_resize(struct ptrvec *ptrvec, size_t size)
{
    ASSUME(ptrvec != NULL);

    if (size <= ptrvec->capacity) {
        for (size_t i = ptrvec->length; i < size; ++i) {
            ptrvec->ptr[i] = NULL;
        }

        ptrvec->length = size;

        return 0;
    }

    if (ERR(ptrvec_reserve(ptrvec, size) != 0)) {
        return -1;
    }

    for (size_t i = ptrvec->length; i < size; ++i) {
        ptrvec->ptr[i] = NULL;
    }

    ptrvec->length = ptrvec->capacity = size;

    return 0;
}

int
ptrvec_reserve(struct ptrvec *ptrvec, size_t size)
{
    ASSUME(ptrvec != NULL);

    if (size <= ptrvec->capacity) {
        return 0;
    }

    if (ERR(vec_reserve_min(&ptrvec->ptr, &ptrvec->capacity,
                            sizeof(*ptrvec->ptr), size - ptrvec->length)
            != 0)) {

        return -1;
    }

    return 0;
}

void
ptrvec_slice(struct ptrvec *ptrvec, size_t begin, size_t end)
{
    ASSUME(ptrvec != NULL);
    ASSUME(begin <= end);
    ASSUME(end <= ptrvec->length);

    ptrvec_remove_r(ptrvec, end, ptrvec->length);
    ptrvec_remove_r(ptrvec, 0, begin);
}

void
ptrvec_free(struct ptrvec *ptrvec)
{
    ASSUME(ptrvec != NULL);

    jfree(ptrvec->ptr);
}

void
ptrvec_delete(struct ptrvec *ptrvec)
{
    ASSUME(ptrvec != NULL);

    for (size_t i = 0; i < ptrvec->length; ++i) {
        jfree(ptrvec->ptr[i]);
    }

    jfree(ptrvec->ptr);
}
