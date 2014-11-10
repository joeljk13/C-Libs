#ifndef VEC_H_
#define VEC_H_ 1

#include "main.h"

/* Reserves exactly size bytes after *ptr, which is a pointer to an array of
 * n * size bytes. Returns 0 on success, nonzero on failure. */
int
vec_reserve_one(void *ptr, size_t n, size_t size);

/* Reserves at least size bytes after *ptr, whcih is a pointer to an array of
 * *n * size bytes. Sets *n to the total number of bytes allocated / size.
 * Returns 0 on success, nonzero on failure. */
int
vec_reserve_one_min(void *ptr, size_t *n, size_t size);

/* Reserves exactly size * extra bytes after *ptr, which is a pointer to an
 * array of n * size bytes. Returns 0 on succes, nonzero on failure. */
int
vec_reserve(void *ptr, size_t n, size_t size, size_t extra);

/* Reserves at least size * extra bytes after *ptr, which is a pointer to an
 * array of *n * size bytes. Sets *n to the total nuumber of bytes allocated /
 * size. Returns 0 on success, nonzero on failure. */
int
vec_reserve_min(void *ptr, size_t *n, size_t size, size_t extra);

/* Shrinks an overallocated array pointed to by *ptr, which is an array of *n *
 * size bytes. Leaves m * size bytes left in the array, in which the original
 * data is preserved. Sets *n to the total allocated bytes / size (i.e. m). */
int
vec_shrink(void *ptr, size_t *n, size_t size, size_t m);

#endif
