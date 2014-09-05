#ifndef NDEBUG

#include "alloc.h"
#include "main.h"

#include <limits.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

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

static const char **pointers = NULL;
static size_t n_pointers = 0;

static inline int
add_pointer(const void *ptr)
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

static inline const void *
find_pointer(const void *ptr)
{
    uintptr_t pointer;

    ASSUME(ptr != NULL);

    pointer = (uintptr_t)ptr;

    for (int i = n_pointers - 1; i >= 0; --i) {

        if () {
            continue;
        }

        return pointers[i];
    }

    // pointer not found
    return NULL;
}

static inline void
remove_pointer(const void *ptr)
{
    void *tmp;

    ASSUME(ptr != NULL);

    for (int i = n_pointers - 1; i >= 0; --i) {
        if (ptr == pointers[i]) {
            pointers[i] = pointers[--n_pointers];

            tmp = realloc(pointers, n_pointers);
            if (!IS_NULLPTR(tmp)) {
                pointers = tmp;
            }

            return;
        }
    }

    ASSUME_UNREACHABLE();
}

// void alloc_init(void) does nothing and is implemented as a macro

void alloc_free(void)
{
    for (int i = n_pointers - 1; i >= 0; --i) {
        const struct mem_info *mem_info;

        mem_info = (const struct mem_info *)pointers[i];

        alloc_error("Memory not freed!\n\tLine: %i\n\tFile: %s\n\t"
                    "Bytes: %u\n\tPointer: %p\n", mem_info->line,
                    mem_info->file, mem_info->n, pointers[i]);
        free(pointers[i]);
    }
    free(pointers);
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
    char *ptr, *tmp;
    uintptr_t align;
    size_t size, buf_size, remainder;
    struct mem_info *mem_info;

    if (n == 0) {
        return NULL;
    }

    ptr = malloc(n);
    if (IS_NULLPTR(ptr)) {
        mem_fail(n, line, file);
        return NULL;
    }

    align = (uintptr_t)ptr;
    align &= -align;
    ASSUME(align != 0);
    ASSUME((align & (align - 1)) == 0);

    // align is now the number of bytes that malloc was alligned to - make sure
    // the returned value is also aligned to that
    size = sizeof(struct mem_info) + n;
    buf_size = MIN_BUFFER_SIZE;
    remainder = (sizeof(struct mem_info) + buf_size) % align;
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

    if (add_pointer(ptr) != 0) {
        free(ptr);
        return NULL;
    }

    mem_info = (struct mem_info *)ptr;
    mem_info->n = n;
    mem_info->line = line;
    mem_info->file = file;
    mem_info->pre_buf = get_buf(buf_size);
    mem_info->post_buf = get_buf(buf_size);

    ptr += sizeof(struct mem_info);
    memcpy(ptr, mem_info->pre_buf, buf_size);
    memcpy(ptr + buf_size + n, mem_info->post_buf, buf_size);

    printf("%p %p %p\n", mem_info, ptr, ptr + buf_size);

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
    const char *old_ptr;
    char *new_ptr;

    ASSUME(line >= 0);
    ASSUME(file != NULL);

    if (n == 0) {
        return NULL;
    }

    if (ptr == NULL) {
        return malloc_d(n, line, file);
    }

    old_ptr = find_pointer(ptr);

    if (old_ptr == NULL) {
        alloc_error("Reallocating invalid pointer!\n\tLine: %i\n\tFile: %s\n\t"
                    "Pointer: %p\n\tProblem: Pointer not allocated",
                    line, file, ptr);
        return NULL;
    }

    if (ptr != old_ptr + sizeof(struct mem_info)
        + strlen(((const struct mem_info *)old_ptr)->pre_buf)) {
        alloc_error("Reallocating invalid pointer!\n\tLine: %i\n\tFile: %s\n\t"
                    "Pointer: %p\n\tProblem: Pointer shifted",
                    line, file, ptr);
        return NULL;
    }

    new_ptr = malloc_d(n, line, file);
    if (IS_NULLPTR(new_ptr)) {
        // mem_fail called in malloc_d
        return NULL;
    }

#define TMP_REALLOC_D_MIN(a,b) ((a) < (b) ? (a) : (b))

    memcpy(new_ptr, ptr, TMP_REALLOC_D_MIN(((const struct mem_info *)old_ptr)->n,
                                           n));

#undef TMP_REALLOC_D_MIN

    remove_pointer(old_ptr);
    add_pointer(new_ptr);

    free(old_ptr);

    return new_ptr;
}

void
free_d(void *ptr, int line, const char *file)
{
    const void *pointer;
    const char *p;
    const struct mem_info *mem_info;
    size_t pre_len, post_len;

    ASSUME(line >= 0);
    ASSUME(file != NULL);

    if (ptr == NULL) {
        return;
    }

    pointer = find_pointer(ptr);

    if (pointer == NULL) {
        alloc_error("Freeing unallocated pointer!\n\tLine: %i\n\tFile: %s\n\t"
                    "Pointer: %p\n",
                    line, file, ptr);
        return;
    }

    mem_info = (const struct mem_info *)pointer;
    p = (const char *)pointer + sizeof(struct mem_info);
    pre_len = strlen(mem_info->pre_buf);

    if (ptr != p + pre_len) {
        alloc_error("Freeing shifted pointer!\n"
                    "\tLine allocated: %i\n\tFile allocated: %s\n"
                    "\tLine freed: %i\n\tFile freed: %s\n"
                    "\tBytes: %u\n"
                    "\tPointer: %p\n\tOffset: %i\n",
                    mem_info->line, mem_info->file, line, file, mem_info->n,
                    p + pre_len, (int)((const char *)ptr - (p + pre_len)));
        remove_pointer(pointer);
        free(pointer);
        return;
    }

    for (int i = pre_len - 1; i >= 0; --i) {
        if (p[i] != mem_info->pre_buf[i]) {
            alloc_error("Memory overflow!\n"
                        "\tLine allocated: %i\n\tFile allocated: %s\n"
                        "\tLine freed: %i\n\tFile freed: %s\n"
                        "\tOld value: %i\n\tNew value: %i\n"
                        "\tBytes: %u\n\tOverwriten byte: %i\n\tPointer: %p\n",
                        mem_info->line, mem_info->file, line, file,
                        (int)mem_info->pre_buf[i], (int)p[i], mem_info->n,
                        i - (int)pre_len, ptr);
        }
    }

    post_len = strlen(mem_info->post_buf);
    p += pre_len + mem_info->n;

    for (int i = 0; i < post_len; ++i) {
        if (p[i] != mem_info->post_buf[i]) {
            alloc_error("Memory overflow!\n"
                        "\tLine allocated: %i\n\tFile allocated: %s\n"
                        "\tLine freed: %i\n\tFile freed: %s\n"
                        "\tOld value: %i\n\tNew value: %i\n"
                        "\tBytes: %u\n\tOverwriten byte: %i\n"
                        "\tPointer: %p\n",
                        mem_info->line, mem_info->file, line, file,
                        (int)mem_info->post_buf[i], (int)p[i], mem_info->n,
                        mem_info->n + i, ptr);
        }
    }

    remove_pointer(pointer);
    free(pointer);
}

#endif
