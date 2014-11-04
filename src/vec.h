#ifndef VEC_H_
#define VEC_H_ 1

#include "main.h"

int
vec_reserve_one(void *ptr, size_t n, size_t size);

int
vec_reserve_one_min(void *ptr, size_t *n, size_t size);

int
vec_reserve(void *ptr, size_t n, size_t size, size_t extra);

int
vec_reserve_min(void *ptr, size_t *n, size_t size, size_t extra);

int
vec_shrink(void *ptr, size_t *n, size_t size, size_t m);
#endif
