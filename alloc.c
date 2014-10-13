#ifndef NDEBUG

#include "main.h"
#include "alloc.h"

#include <limits.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define INIT_ALLOC_MIN_BUF_SIZE 32

static size_t alloc_min_buf_size = INIT_ALLOC_MIN_BUF_SIZE;

void
alloc_size(size_t size)
{
    ASSUME(alloc_min_buf_size >= INIT_ALLOC_MIN_BUF_SIZE);

    if (size <= alloc_min_buf_size) {
        return;
    }

    // Round up to the next power of 2
    --size;
    for (size_t i = 1; i < sizeof(size) * CHAR_BIT; i *= 2) {
        size |= size >> i;
    }
    alloc_min_buf_size = size + 1;
}

struct mem_info {
    size_t bytes;
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
static void
mem_fail(size_t bytes, int line, const char *file)
{
    ASSUME(line >= 0);
    ASSUME(file != NULL);

    alloc_error("Memory failure!\n\tLine: %i\n\tFile: %s\n\tBytes: %u\n",
                line, file, bytes);
}

struct ptr_info {
    const void *ptr;
    size_t bytes;
};

static struct ptr_info *ptr_infos = NULL;
static size_t n_ptr_infos = 0;

static inline int
add_ptr_info(const void *ptr, size_t bytes)
{
    void *tmp;

    ASSUME(ptr != NULL);
    ASSUME(bytes > 0);

    tmp = realloc(ptr_infos, n_ptr_infos + 1);
    if (ERR(tmp == NULL)) {
        mem_fail(n_ptr_infos + 1, __LINE__, __FILE__);
        return -1;
    }

    ptr_infos = tmp;
    ptr_infos[n_ptr_infos].ptr = ptr;
    ptr_infos[n_ptr_infos].bytes = bytes;
    ++n_ptr_infos;

    return 0;
}

static inline const void *
find_ptr_info(const void *ptr)
{
    ASSUME(ptr != NULL);

    // Lookups are more likely to be from the more recently allocated pointers
    for (int i = n_ptr_infos - 1; i >= 0; --i) {
        for (const char *p = ptr_infos[i].ptr;
             p != ptr_infos[i].ptr + ptr_infos[i].bytes;
             ++p) {

            if (p == ptr) {
                return ptr_infos[i].ptr;
            }
        }
    }

    // pointer not found
    return NULL;
}

static inline void
remove_ptr_info(const void *ptr)
{
    ASSUME(ptr != NULL);

    for (int i = n_ptr_infos - 1; i >= 0; --i) {
        void *tmp;

        if (ptr != ptr_infos[i].ptr) {
            continue;
        }

        // The order doesn't really matter, so just replace this with the
        // last ptr_info and remove the last
        ptr_infos[i] = ptr_infos[--n_ptr_infos];

        if (n_ptr_infos == 0) {
            free(ptr_infos);
            ptr_infos = NULL;
        } else {
            tmp = realloc(ptr_infos, n_ptr_infos);
            if (ERR(tmp == NULL)) {
                mem_fail(n_ptr_infos, __LINE__, __FILE__);
                return;
            }
            ptr_infos = tmp;
        }

        return;
    }

    ASSUME_UNREACHABLE();
}

// void alloc_init(void) does nothing

void
alloc_free(void)
{
    for (int i = n_ptr_infos - 1; i >= 0; --i) {
        const struct mem_info *mem_info;

        mem_info = (const struct mem_info *)ptr_infos[i].ptr;

        alloc_error("Memory not freed!\n\tLine: %i\n\tFile: %s\n"
                    "\tBytes: %u\n\tPointer: %p\n",
                    mem_info->line, mem_info->file, mem_info->bytes,
                    ptr_infos[i]);

        free((void *)ptr_infos[i].ptr);
    }

    free(ptr_infos);
}

static inline const char *
get_buf(size_t len)
{
    char *str;
    size_t size;

    ASSUME(len >= alloc_min_buf_size);

    size = (len + 1) * sizeof(*str);

    str = malloc(size);
    if (ERR(str == NULL)) {
        mem_fail(size, __LINE__, __FILE__);
        return NULL;
    }

    for (int i = 0; i < len; ++i) {
        do {
            str[i] = rand() % (CHAR_MAX - CHAR_MIN) + CHAR_MIN;
        } while (str[i] == '\0');
    }

    str[len] = '\0';

    return str;
}

void *
malloc_d(size_t n, int line, const char *file)
{
    char *ptr, *tmp;
    uintptr_t align;
    size_t size, buf_size, remainder;
    struct mem_info *mem_info;

    ASSUME(line >= 0);
    ASSUME(file != NULL);

    if (n == 0) {
        return NULL;
    }

    ptr = malloc(n);
    if (ERR(ptr == NULL)) {
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
    buf_size = alloc_min_buf_size;
    remainder = (sizeof(struct mem_info) + buf_size) % align;
    if (remainder) {
        buf_size += remainder;
    }
    size += buf_size * 2;

    tmp = realloc(ptr, size);
    if (ERR(tmp == NULL)) {
        free(ptr);
        mem_fail(size, line, file);

        return NULL;
    }
    ptr = tmp;

    if (add_ptr_info(ptr, size) != 0) {
        free(ptr);
        return NULL;
    }

    mem_info = (struct mem_info *)ptr;
    mem_info->bytes = n;
    mem_info->line = line;
    mem_info->file = file;
    mem_info->pre_buf = get_buf(buf_size);
    mem_info->post_buf = get_buf(buf_size);

    ptr += sizeof(struct mem_info);
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
    if (ERR(ptr == NULL)) {
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
    const struct mem_info *mem_info;

    ASSUME(line >= 0);
    ASSUME(file != NULL);

    if (ptr == NULL) {
        return malloc_d(n, line, file);
    }

    if (n == 0) {
        return NULL;
    }

    old_ptr = find_ptr_info(ptr);
    if (old_ptr == NULL) {
        alloc_error("Reallocating invalid pointer!\n\tLine: %i\n\tFile: %s\n"
                    "\tPointer: %p\n\tProblem: Pointer not allocated",
                    line, file, ptr);

        return NULL;
    }

    mem_info = (const struct mem_info *)old_ptr;

    if (ptr != old_ptr + sizeof(struct mem_info)
        + strlen((mem_info->pre_buf))) {

        alloc_error("Reallocating invalid pointer!\n\tLine: %i\n\tFile: %s\n"
                    "\tPointer: %p\n\tProblem: Pointer shifted",
                    line, file, ptr);

        return NULL;
    }

    new_ptr = malloc_d(n, line, file);
    if (ERR(new_ptr == NULL)) {
        // mem_fail called in malloc_d
        return NULL;
    }

    memcpy(new_ptr, ptr, mem_info->bytes < n ? mem_info->bytes : n);

    remove_ptr_info(old_ptr);
    free((void *)old_ptr);

    return new_ptr;
}

void
free_d(const void *ptr, int line, const char *file)
{
    const char *ptr_info;
    const char *p;
    const struct mem_info *mem_info;
    size_t pre_len, post_len;

    ASSUME(line >= 0);
    ASSUME(file != NULL);

    if (ptr == NULL) {
        return;
    }

    ptr_info = find_ptr_info(ptr);
    if (ptr_info == NULL) {
        alloc_error("Freeing unallocated pointer!\n\tLine: %i\n\tFile: %s\n"
                    "\tPointer: %p\n",
                    line, file, ptr);

        return;
    }

    mem_info = (const struct mem_info *)ptr_info;
    p = ptr_info + sizeof(struct mem_info);
    pre_len = strlen(mem_info->pre_buf);

    if (ptr != p + pre_len) {
        alloc_error("Freeing shifted pointer!\n"
                    "\tLine allocated: %i\n\tFile allocated: %s\n"
                    "\tLine freed: %i\n\tFile freed: %s\n"
                    "\tBytes: %u\n"
                    "\tPointer: %p\n\tOffset: %td\n",
                    mem_info->line, mem_info->file, line, file, mem_info->bytes,
                    p + pre_len, (const char *)ptr - (p + pre_len));

        remove_ptr_info(ptr_info);
        free((void *)ptr_info);

        return;
    }

    // Start closer to the actual data, because overflows are more likely there
    for (int i = pre_len - 1; i >= 0; --i) {
        if (p[i] != mem_info->pre_buf[i]) {
            alloc_error("Memory overflow!\n"
                        "\tLine allocated: %i\n\tFile allocated: %s\n"
                        "\tLine freed: %i\n\tFile freed: %s\n"
                        "\tOld value: %i\n\tNew value: %i\n"
                        "\tBytes: %u\n\tOverwriten byte: %i\n\tPointer: %p\n",
                        mem_info->line, mem_info->file, line, file,
                        (int)mem_info->pre_buf[i], (int)p[i], mem_info->bytes,
                        i - (int)pre_len, ptr);
        }
    }

    post_len = strlen(mem_info->post_buf);
    p += pre_len + mem_info->bytes;

    for (int i = 0; i < post_len; ++i) {
        if (p[i] != mem_info->post_buf[i]) {
            alloc_error("Memory overflow!\n"
                        "\tLine allocated: %i\n\tFile allocated: %s\n"
                        "\tLine freed: %i\n\tFile freed: %s\n"
                        "\tOld value: %i\n\tNew value: %i\n"
                        "\tBytes: %u\n\tOverwriten byte: %i\n"
                        "\tPointer: %p\n",
                        mem_info->line, mem_info->file, line, file,
                        (int)mem_info->post_buf[i], (int)p[i], mem_info->bytes,
                        mem_info->bytes + i, ptr);
        }
    }

    remove_ptr_info(ptr_info);
    free((void *)ptr_info);
}

#endif
