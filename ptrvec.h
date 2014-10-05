#ifndef VECTOR_H_
#define VECTOR_H_ 1

#include "main.h"

/* A vector of pointers. */
struct ptrvec {
    void **ptr;
    size_t length;
    size_t capacity;
}

/* All of the following functions take a struct ptrvec * as their first
 * argument. This pointer is always assumed not to be NULL.
 *
 * Any functions that take an index or multiple indices assume the the indices
 * are valid, and that begin <= end.
 *
 * Note: the pointers contained in a ptrvec are not managed by the ptrvec. */

/* Initializes the ptrvec. Returns 0 on success, -1 on failure. */
int
ptrvec_init(struct ptrvec *ptrvec);

/* Sets all the pointers to NULL. */
void
ptrvec_zero(struct ptrvec *ptrvec);

/* Appends ptr to the end of ptrvec. Returns 0 on success, -1 on failure. */
int
ptrvec_push(struct ptrvec *ptrvec, void *ptr);

/* Appends each pointer in ptr to ptrvec. Returns 0 on success, -1 on failure. */
int
ptrvec_push_v(struct ptrvec *ptrvec, struct ptrvec *ptr);

/* Removes the last pointer from ptrvec and returns it. Assumes ptrvec is not
 * empty. */
void *
ptrvec_pop(struct ptrvec *ptrvec);

/* Returns the last pointer in ptrvec. Assumes ptrvec is not empty. */
void *
ptrvec_peek(struct ptrvec *ptrvec);

/* Inserts ptr into ptrvec at index, shifting all pointers at and after index
 * by one. Returns 0 on success, -1 on failure. */
int
ptrvec_insert(struct ptrvec *ptrvec, void *ptr, size_t index);

/* Inserts each pointer in ptr to ptrvec starting at index, shifting all
 * pointers after index in ptrvec by ptr->length. Returns 0 on succes, -1 on
 * failure. */
int
ptrvec_insert_v(struct ptrvec *ptrvec, struct ptrvec *ptr, size_t index);

/* Removes the pointer at index from ptrvec, shifting each pointer after index
 * by one. */
void
ptrvec_remove(struct ptrvec *ptrvec, size_t index);

/* Removes the pointers in [begin, end), and shifts the pointers starting at
 * end by (end - begin). */
void
ptrvec_remove_r(struct ptrvec *ptrvec, size_t begin, size_t end);

/* Same as ptrvec_remove, but faster. However, this doesn't preserve the
 * ordering of the pointers. */
void
ptrvec_remove_fast(struct ptrvec *ptrvec, size_t index);

/* Same as ptrvec_remove_fast, but faster, at least for small ranges. However,
 * this doesn't preserve the ordering of the pointers. */
void
ptrvec_remove_fast_r(struct ptrvec *ptrvec, size_t begin, size_t end);

/* Returns 1 if ptrvec contains ptr, otherwise returns 0. */
int
ptrvec_contains(struct ptrvec *ptrvec, void *ptr);

/* Returns the index where ptr is. If ptr is not in ptrvec, returns
 * ptrvec->lengh. */
size_t
ptrvec_find(struct ptrvec *ptrvec, void *ptr);

/* Resizes ptrvec to size. Any new pointers added are set to NULL. Returns 0 on
 * success, -1 on failure. */
int
ptrvec_resize(struct ptrvec *ptrvec, size_t size);

/* Reserves enough memory for at least size pointers. Returns 0 on success, -1
 * on failure. */
int
ptrvec_reserve(struct ptrvec *ptrvec, size_t size);

/* Removes all pointers outside the range [begin, end). Returns 0 on success,
 * -1 on failure. */
int
ptrvec_slice(struct ptrvec *ptrvec, size_t begin, size_t end);

/* Frees the memory used by ptrvec. */
void
ptrvec_free(struct ptrvec *ptrvec);

#endif
