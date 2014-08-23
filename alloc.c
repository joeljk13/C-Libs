#ifndef NDEBUG

#include "alloc.h"
#include "main.h"

#include <stdint.h>

#define MIN_BUFFER_SIZE 8

struct mem_info {
    size_t n;
    int line;
    const char *file;
    const char *pre_buf;
    const char *post_buf;
};

static inline void
mem_fail(size_t bytes, int line, const char *file)
{
    ASSUME(line >= 0);
    ASSUME(!IS_NULLPTR(file));
    fprintf(stderr, "Memory failure!\n\tLine: %i\n\tFile: %s\n\tBytes: %u",
            line, file, bytes);
    exit(EXIT_FAILURE);
}

struct pointer {
    void *begin;
    size_t bytes;
};

static struct pointer *pointers = NULL;
static size_t n_pointers = 0;

static inline void
add_pointer(void *ptr)
{
    if (IS_NULLPTR(pointers)) {
        pointers = malloc(sizeof(*pointers));
        if (IS_NULLPTR(pointers)) {
            // The line won't be exactly right, but it's close enough
            mem_fail(__LINE__, __FILE__);
            return;
        }
    }
    else if (n_pointers) {

    }
}

static inline const char *
get_buf(int len)
{
    char *str;

    str = malloc(len * sizeof(*str));
    if (IS_NULLPTR(str)) {
        // The line won't be exactly right, but it's close enough
        mem_fail(__LINE__, __FILE__);
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
    char *ptr;
    uintptr_t align;
    size_t size, buf_size, remainder;
    struct mem_info *mem_info;

    ptr = malloc(n);
    if (IS_NULLPTR(ptr)) {
        mem_fail(line, file);
        return NULL;
    }

    align = (uintptr_t)ptr;
    align &= -align;
    ASSUME(align != 0);
    ASSUME(align & (align - 1) == 0);
    // align is now the number of bytes that malloc was alligned to - make sure
    // the returned value is also aligned to that

    size = sizeof(mem_info) + n;
    buf_size = MIN_BUFFER_SIZE;
    remainder = (sizeof(mem_info) + buf_size) % align;
    if (remainder) {
        buf_size += remainder;
    }
    size += buf_size * 2;

    ptr = realloc(ptr, size);
    if (IS_NULLPTR(ptr)) {
        mem_fail(line, file);
        return NULL;
    }

    add_pointer(ptr);

    mem_info = (struct mem_info *)ptr;
    mem_info->n = n;
    mem_info->line = line;
    mem_info->file = file;
    mem_info->pre_buf = get_buf(buf_size);
    mem_info->post_buf = get_buf(buf_size);

    ptr += sizeof(mem_info);
    memcpy(ptr, mem_info->pre_buf, buf_size);
    memcpy(ptr + buf_size + n, mem_info->post_buf, buf_size);

    return ptr + buf_size;
}

void *
calloc_d(size_t n, size_t size, int line, const char *file)
{
    void *ptr;

    ptr = malloc_d(n * size, line, file);
    if (IS_NULLPTR(ptr)) {
        // mem_fail already called in malloc_d
        return NULL;
    }

    memset(ptr, 0, n * size);

    return ptr;
}

void *
realloc_d(void *ptr, size_t n, int line, const char *file)
{
    TODO(Implement realloc_d);
}

void
free_d(void *ptr, int line, const char *file)
{
    TODO(Implement free_d);
}

#endif
