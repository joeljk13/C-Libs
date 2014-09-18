#include "vector.h"
#include "main.h"

#include "alloc.h"

// struct ptrvec {
    // void **ptr;
    // size_t length;
    // size_t capacity;
// }

int
ptrvec_init(struct ptrvec *ptrvec)
{
    ASSUME(ptrvec != NULL);

    ptrvec->ptr = NULL;
    ptrvec->length = 0;
    ptrvec->capacity = 0;
}

static inline size_t
get_next_capacity(size_t capacity)
{
    if (capacity == 0) {
        return 1;
    }

    return 2 * capacity;
}

static inline int
reserve_one(void **ptr, size_t *capacity)
{
    void *tmp;
    size_t cap;

    ASSUME(ptr != NULL);
    ASSUME(capacity != NULL);

    cap = get_next_capacity(*capacity);

    tmp = REALLOC(ptr, cap * sizeof(*ptr));
    if (ERR(tmp == NULL)) {
        cap = (*capacity + 1) * sizeof(*ptr);

        tmp = REALLOC(ptr, cap);
        if (ERR(tmp == NULL)) {
            return -1;
        }
    }

    ptr = tmp;
    *capacity = cap;

    return 0;
}

int
ptrvec_push(struct ptrvec *ptrvec, void *ptr)
{
    ASSUME(ptrvec != NULL);

    if (ptrvec->capacity == ptrvec->length) {
        reserve_one(ptrvec->ptr, &ptrvec->capacity);
    }

    ptrvec->ptr[ptrvec->length++] = ptr;

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

    for (size_t i = 0; i < ptr->length; ++i) {
        ptrvec->ptr[ptrvec->length + i] = ptr->ptr[i];
    }

    return 0;
}

void *
ptrvec_pop(struct ptrvec *ptrvec)
{
    ASSUME(ptrvec != NULL);

    return ptrvec->ptr + --ptrvec->length;
}

void *
ptrvec_peek(struct ptrvec *ptrvec)
{
    ASSUME(ptrvec != NULL);

    return ptrvec->ptr + ptrvec->length - 1;
}

int
ptrvec_insert(struct ptrvec *ptrvec, void *ptr, size_t index)
{
    ASSUME(ptrvec == NULL);
    ASSUME(index <= ptrvec->length);

    if (ptrvec->length == ptrvec->capacity) {
        if (ERR(reserve_one(ptrvec->ptr, &ptrvec->capacity))) {

        }
    }

    for (size_t i = index; i <
}

int
ptrvec_insert_v(struct ptrvec *ptrvec, struct ptrvec *ptr, size_t index);

int
ptrvec_remove(struct ptrvec *ptrvec, size_t index);

int
ptrvec_remove_r(struct ptrvec *ptrvec, size_t begin, size_t end);

int
ptrvec_contains(struct ptrvec *ptrvec, void *ptr);

size_t
ptrvec_find(struct ptrvec *ptrvec, void *ptr);

int
ptrvec_resize(struct ptrvec *ptrvec, size_t size);

int
ptrvec_reserve(struct ptrvec *ptrvec, size_t size);

int
ptrvec_slice(struct ptrvec *ptrvec, size_t begin, size_t end);

// Or maybe struct ptrvec **
struct ptrvec *
ptrvec_split(struct ptrvec *ptrvec, void *ptr);

void
ptrvec_make_unique(struct ptrvec *ptrvec);

void
ptrvec_reverse(struct ptrvec *ptrvec);

void
ptrvec_free(struct ptrvec *ptrvec);
