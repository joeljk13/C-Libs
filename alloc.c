#ifndef NDEBUG

#include "alloc.h"
#include "main.h"

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>

#define MIN_BUFFER_SIZE 8

struct mem_info {
    size_t n;
    int line;
    const char *file;
    const char *pre_buf;
    const char *post_buf;
};

static inline void
alloc_error(const char *format, ...)
{
    va_list vl;

    va_start(vl, format);

    vfprintf(stderr, format, vl);

    va_end(vl);
}

// The line might not always be exactly right, but it should be close enough to
// identify where the error occured
static inline void
mem_fail(size_t bytes, int line, const char *file)
{
    ASSUME(line >= 0);
    ASSUME(!IS_NULLPTR(file));

    alloc_error("Memory failure!\n\tLine: %i\n\tFile: %s\n\tBytes: %u\n",
            line, file, bytes);
}

static char **pointers = NULL;
static size_t n_pointers = 0;

static inline int
add_pointer(void *ptr)
{
    void *tmp;

    tmp = realloc(pointers, n_pointers + 1);
    if (IS_NULLPTR(tmp)) {
        mem_fail(n_pointers + 1, __LINE__, __FILE__);
        return 1;
    }

    pointers = tmp;
    pointers[n_pointers] = ptr;
    ++n_pointers;

    return 0;
}

static inline void *
find_pointer(void *ptr)
{
    ASSUME(ptr != NULL);

    for (int i = n_pointers - 1; i >= 0; --i) {
        // Should this compare uintptr_t, or void *?
        if ((uintptr_t)pointers[i] > (uintptr_t)ptr
            || (uintptr_t)(pointers[i]
                           + ((const struct mem_info *)pointers[i])->bytes)
            > (uintptr_t)ptr) {
            continue;
        }

        return pointers[i];
    }

    // pointer not found
    return NULL;
}

static inline const char *
get_buf(int len)
{
    char *str;

    str = malloc(len * sizeof(*str));
    if (IS_NULLPTR(str)) {
        mem_fail(len * sizeof(*str), __LINE__, __FILE__);
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
    char *ptr, tmp;
    uintptr_t align;
    size_t size, buf_size, remainder;
    struct mem_info *mem_info;

    ptr = malloc(n);
    if (IS_NULLPTR(ptr)) {
        mem_fail(n, line, file);
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

    tmp = realloc(ptr, size);
    if (IS_NULLPTR(tmp)) {
        free(ptr);
        mem_fail(size, line, file);
        return NULL;
    }
    ptr = tmp;

    if (add_pointer(ptr, size) != 0) {
        free(ptr);
        return NULL;
    }

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

    ASSUME(line >= 0);
    ASSUME(file != NULL);

    ptr = malloc_d(n * size, line, file);
    if (IS_NULLPTR(ptr)) {
        // mem_fail called in malloc_d
        return NULL;
    }

    memset(ptr, 0, n * size);

    return ptr;
}

void *
realloc_d(void *ptr, size_t n, int line, const char *file)
{
    void *new_ptr;

    ASSUME(line >= 0);
    ASSUME(file != NULL);

    if (ptr == NULL) {
        return malloc_d(n, line, file);
    }

    if (n == 0) {
        return NULL;
    }

    new_ptr = malloc_d(n, line, file);
    if (IS_NULLPTR(new_ptr)) {
        // mem_fail called in malloc_d
        return NULL;
    }

    TODO(Implement realloc_d);
}

void
free_d(void *ptr, int line, const char *file)
{
    ASSUME(line >= 0);
    ASSUME(file != NULL);

    if (ptr == NULL) {
        return;
    }

    TODO(Implement free_d);
}

#endif
