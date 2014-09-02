#ifndef NDEBUG

#include "alloc.h"
#include "main.h"

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

// The 1 pointer cache helps for functions that call both find_pointer and
// remove_pointer; otherwise there's redundant searching
static void *find_pointer_cache = NULL;

static inline void *
find_pointer(void *ptr)
{
    ASSUME(ptr != NULL);

#define TMP_FIND_POINTER_CMP(p) (uintptr_t)(p) > (uintptr_t)ptr \
    || (uintptr_t)((p) + ((const struct mem_info *)(p))->bytes) \
    > (uintptr_t)ptr

    if (!TMP_FIND_POINTER_CMP(find_pointer_cache)) {
        return find_pointer_cache;
    }

    for (int i = n_pointers - 1; i >= 0; --i) {
        // Should this compare uintptr_t, or void *?
        if (TMP_FIND_POINTER_CMP(pointers[i])) {
            continue;
        }

        find_pointer_cache = pointers[i];

        return pointers[i];
    }

#undef TMP_FIND_POINTER_CMP

    // pointer not found
    return NULL;
}

static inline void
remove_pointer(void *ptr)
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

    ASSUME(UNREACHABLE);
}

void
alloc_init(void)
{ }

void alloc_free(void)
{
    for (int i = n_pointers - 1; i >= 0; --i) {
        TODO("Add more!");
        alloc_error("Memory not freed!");
    }
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

    if (n == NULL) {
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
    char *old_ptr, new_ptr;

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
        + strlen((const struct mem_info *)old_ptr->pre_buf)) {
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

    memcpy(new_ptr, ptr, TMP_REALLOC_D_MIN((const struct mem_info *)old_ptr->n,
                                           n));

#undef TMP_REALLOC_D_MIN

    remove_pointer(old_ptr);
    add_pointer(new_pointer);

    free(old_ptr);

    return new_ptr;
}

void
free_d(void *ptr, int line, const char *file)
{
    void *pointer;

    ASSUME(line >= 0);
    ASSUME(file != NULL);

    if (ptr == NULL) {
        return;
    }

    pointer = find_pointer(ptr);

    if (pointer == NULL) {
        alloc_error("Freeing invalid pointer!\n\tLine: %i\n\tFile: %s\n\t"
                    "Pointer: %p\n\tProblem: Pointer not allocated",
                    line, file, ptr);
        return NULL;
    }

    if (ptr != pointer + sizeof(struct mem_info)
        + strlen((const struct mem_info *)pointer->pre_buf)) {
        alloc_error("Freeing invalid pointer!\n\tLine: %i\n\tFile: %s\n\t"
                    "Pointer: %p\n\tProblem: Pointer shifted",
                    line, file, ptr);
        return NULL;
    }

    TODO("Check for buffer overrun");

    remove_pointer(pointer);

    free(pointer);
}

#endif
