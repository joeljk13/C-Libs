#ifndef NDEBUG

#include "alloc.h"

struct mem_info {
    size_t n;
    const char *pre_buf;
    const char *post_buf;
};

struct pointer {
    void *begin;
    size_t bytes;
};

static struct pointer *pointers = NULL;

void *
malloc_d(size_t n, int line, const char *file)
{

}

void *
calloc_d(size_t n, size_t size, int line, const char *file)
{

}

void *
realloc_d(void *ptr, size_t n, int line, const char *file)
{

}

void
free_d(void *ptr, int line, const char *file)
{

}

#endif
