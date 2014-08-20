#ifndef NDEBUG

#include "alloc.h"

#include <stdint.h>

#define MIN_BUFFER_SIZE 8

/* REQUIRES: GCC */
#define assume(b) do { \
    if (!(b)) { \
        __builtin_unreachable(); \
    } \
} while (0)

static inline is_nullptr(void *ptr)
{
    return __builtin_expect(ptr == NULL, 0);
}

struct mem_info {
    size_t n;
    int line;
    const char *file;
    const char *pre_buf;
    const char *post_buf;
};

struct pointer {
    void *begin;
    size_t bytes;
};

static struct pointer *pointers = NULL;

static void
add_pointer(void *ptr)
{

}

static inline const char *
get_buf(int len)
{
    char *str = malloc(len * sizeof(char));
    if (str == NULL) {
        return NULL;
    }
    for (int i = 0; i < len; ++i) {
        str[i] = rand() % (CHAR_MAX - CHAR_MIN) + CHAR_MIN;
    }
    return str;
}

void *
malloc_d(size_t n, int line, const char *file)
{
    const char *ptr = malloc(n);
    if (ptr == NULL) {
        return NULL;
    }
    uintptr_t align = (uintptr_t)ptr;
    align &= -align;
    // align is now the number of bytes that malloc was alligned to - make sure
    // the returned value is also aligned to that
    free(ptr);
    size_t size = sizeof(mem_info) + n;
    size_t buf_size = MIN_BUFFER_SIZE;
    size_t remainder = (sizeof(mem_info) + buf_size) % align;
    if (remainder) {
        buf_size += remainder;
    }
    size += buf_size * 2;
    ptr = malloc(size);
    if (ptr == NULL) {
        return NULL;
    }
    struct mem_info *mem_info = ptr;
    mem_info->n = n;
    mem_info->line = line;
    mem_info->file = file;
    mem_info->pre_buf = get_buf(buf_size);
    mem_info->post_buf = get_buf(buf_size);
    ptr += sizeof(mem_info) + BUFFER_SIZE;
    add_pointer(ptr);
    return mem_info + 1;
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
