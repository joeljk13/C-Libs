#include "main.h"
#include "alloc.h"

#include <limits.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INIT_ALLOC_MIN_BUF_SIZE 32

#ifdef JEMALLOC

#define MALLOC(n) jemalloc(n)
#define CALLOC(n,s) jecalloc((n), (s))
#define REALLOC(p,n) jerealloc((p), (n))
#define FREE(p) jefree((void *)(p))

#else

#define MALLOC(n) malloc(n)
#define CALLOC(n,s) calloc((n), (s))
#define REALLOC(p,n) realloc((p), (n))
#define FREE(p) free((void *)(p))

#endif

#ifndef NDEBUG

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

// The line might not always be exactly right, but it should be close enough to
// identify where the error occured
static void NONNULL
mem_fail(size_t bytes, int line, const char *file)
{
    ASSUME(line >= 0);
    ASSUME(file != NULL);

    fprintf(stderr, "Memory failure!\n\tLine: %i\n\tFile: %s\n\tBytes: %u\n",
            line, file, bytes);
}

// TODO - Optimize the pointer info storage. Right now, it each pointer
// addition/removal causes an allocation/deallocation, which shouldn't be
// necessary. Maybe adapt ptrvec to a vector of struct ptr_info? Or maybe use a
// tree to get O(log(n)) insertion, removal, and deletion?
struct ptr_info {
    const void *ptr;
    size_t bytes;
};

static struct ptr_info *ptr_infos = NULL;
static size_t n_ptr_infos = 0;

static inline int NONNULL
add_ptr_info(const void *ptr, size_t bytes)
{
    struct ptr_info *tmp;

    ASSUME(ptr != NULL);
    ASSUME(bytes > 0);

    tmp = REALLOC(ptr_infos, (n_ptr_infos + 1) * sizeof(*tmp));
    if (ERR(tmp == NULL)) {
        mem_fail((n_ptr_infos + 1) * sizeof(*tmp), __LINE__, __FILE__);
        return -1;
    }
    ptr_infos = tmp;

    ptr_infos[n_ptr_infos].ptr = ptr;
    ptr_infos[n_ptr_infos].bytes = bytes;
    ++n_ptr_infos;

    return 0;
}

static inline const void * NONNULL
find_ptr_info(const void *ptr)
{
    ASSUME(ptr != NULL);

    // Lookups are more likely to be from the more recently allocated pointers
    for (size_t i = 0; i < n_ptr_infos; ++i) {
        for (const char *p = ptr_infos[i].ptr, p_end = p + ptr_infos[i].bytes;
             p != p_end; ++p) {
            if (p == ptr) {
                return ptr_infos[i].ptr;
            }
        }
    }

    // Pointer not found
    return NULL;
}

static inline int NONNULL
remove_ptr_info(const void *ptr)
{
    ASSUME(ptr != NULL);

    for (size_t i = 0; i < n_ptr_infos; ++i) {
        struct ptr_info *tmp;

        if (ptr != ptr_infos[i].ptr) {
            continue;
        }

        // The order doesn't really matter, so just replace this with the
        // last ptr_info and remove the last
        ptr_infos[i] = ptr_infos[--n_ptr_infos];

        if (n_ptr_infos == 0) {
            FREE(ptr_infos);
            ptr_infos = NULL;
        } else {
            tmp = REALLOC(ptr_infos, n_ptr_infos * sizeof(*tmp));
            if (ERR(tmp == NULL)) {
                mem_fail(n_ptr_infos * sizeof(*tmp), __LINE__, __FILE__);
                return -1;
            }
            ptr_infos = tmp;
        }

        return 0;
    }

    ASSUME_UNREACHABLE();

    return -1;
}

// alloc_init() does nothing

void
alloc_free(void)
{
    for (size_t i = 0; i < n_ptr_infos; ++i) {
        const struct mem_info *mem_info =
            (const struct mem_info *)ptr_infos[i].ptr;

        fprintf(stderr, "Memory not freed!\n\tLine: %i\n\tFile: %s\n"
                "\tBytes: %u\n\tPointer: %p\n",
                mem_info->line, mem_info->file, mem_info->bytes,
                ptr_infos[i]);

        FREE(ptr_infos[i].ptr);
    }

    FREE(ptr_infos);
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

    for (size_t i = 0; i < len; ++i) {
        do {
            str[i] = rand() % (CHAR_MAX - CHAR_MIN) + CHAR_MIN;
        } while (str[i] == '\0');
    }

    str[len] = '\0';

    return str;
}

static void * NONNULL
alloc_d(size_t n, int clear, int line, const char *file)
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

    ptr = clear ? CALLOC(n, 1) : MALLOC(n);
    if (ERR(ptr == NULL)) {
        mem_fail(n, line, file);
        return NULL;
    }

    // TODO - optimize align into bitwise ops

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

    tmp = REALLOC(ptr, size);
    if (ERR(tmp == NULL)) {
        FREE(ptr);
        mem_fail(size, line, file);

        return NULL;
    }
    ptr = tmp;

    if (clear) {
        memset(ptr + n, 0, size - n);
    }

    if (add_ptr_info(ptr, size) != 0) {
        FREE(ptr);
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
malloc_d(size_t n, int line, const char *file)
{
    ASSUME(line >= 0);
    ASSUME(file != NULL);

    return alloc_d(n, 0, line, file);
}

void *
calloc_d(size_t n, size_t size, int line, const char *file)
{
    ASSUME(line >= 0);
    ASSUME(file != NULL);

    // TODO - check for overflow in n * size

    return alloc_d(n * size, 1, line, file);
}

void *
realloc_d(void *ptr, size_t n, int line, const char *file)
{
    const char *old_ptr;
    char *new_ptr;
    struct mem_info *mem_info;

    ASSUME(line >= 0);
    ASSUME(file != NULL);

    if (ptr == NULL) {
        return malloc_d(n, line, file);
    }

    if (ERR(n == 0)) {
        fprintf(stderr, "Warning: realloc(ptr, 0) is not portable, since it\n"
                "may or may not free ptr.\n\tLine: %i\n\tFile: %s\n"
                "\tPointer: %p\n",
                line, file, ptr);

        free_d(ptr, line, file);

        return NULL;
    }

    old_ptr = find_ptr_info(ptr);
    if (ERR(old_ptr == NULL)) {
        fprintf(stderr,
                "Reallocating invalid pointer!\n\tLine: %i\n\tFile: %s\n"
                "\tPointer: %p\n\tProblem: Pointer not allocated\n",
                line, file, ptr);

        return NULL;
    }

    mem_info = (struct mem_info *)old_ptr;

    if (ERR(ptr != old_ptr + sizeof(struct mem_info)
        + strlen((mem_info->pre_buf)))) {

        fprintf(stderr,
                "Reallocating invalid pointer!\n\tLine: %i\n\tFile: %s\n"
                "\tPointer: %p\n\tProblem: Pointer shifted\n",
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
    FREE(old_ptr);

    return new_ptr;
}

static int
do_free_d(void *ptr, int line, const char *file)
{
    int err;
    const char *ptr_info;
    const char *p;
    const struct mem_info *mem_info;
    size_t pre_len, post_len;

    ASSUME(line >= 0);
    ASSUME(file != NULL);

    if (ptr == NULL) {
        return 0;
    }

    ptr_info = find_ptr_info(ptr);
    if (ERR(ptr_info == NULL)) {
        fprintf(stderr, "Freeing unallocated pointer!\n\tLine: %i\n"
                "\tFile: %s\n\tPointer: %p\n",
                line, file, ptr);

        return -1;
    }

    mem_info = (const struct mem_info *)ptr_info;
    p = ptr_info + sizeof(struct mem_info);
    pre_len = strlen(mem_info->pre_buf);

    err = 0;

    if (ERR(ptr != p + pre_len)) {
        fprintf(stderr, "Freeing shifted pointer!\n"
                "\tLine allocated: %i\n\tFile allocated: %s\n"
                "\tLine freed: %i\n\tFile freed: %s\n"
                "\tBytes: %u\n"
                "\tPointer: %p\n\tOffset: %td\n",
                mem_info->line, mem_info->file, line, file, mem_info->bytes,
                p + pre_len, (const char *)ptr - (p + pre_len));

        err = -1;
    }

    for (size_t i = 0; i < pre_len; ++i) {
        if (ERR(p[i] != mem_info->pre_buf[i])) {
            fprintf(stderr, "Memory overflow!\n"
                    "\tLine allocated: %i\n\tFile allocated: %s\n"
                    "\tLine freed: %i\n\tFile freed: %s\n"
                    "\tOld value: %i\n\tNew value: %i\n"
                    "\tBytes: %u\n\tOverwriten byte: %i\n\tPointer: %p\n",
                    mem_info->line, mem_info->file, line, file,
                    (int)mem_info->pre_buf[i], (int)p[i], mem_info->bytes,
                    i - (int)pre_len, ptr);

            err = -1;
        }
    }

    post_len = strlen(mem_info->post_buf);
    p += pre_len + mem_info->bytes;

    for (size_t i = 0; i < post_len; ++i) {
        if (ERR(p[i] != mem_info->post_buf[i])) {
            fprintf(stderr, "Memory overflow!\n"
                        "\tLine allocated: %i\n\tFile allocated: %s\n"
                        "\tLine freed: %i\n\tFile freed: %s\n"
                        "\tOld value: %i\n\tNew value: %i\n"
                        "\tBytes: %u\n\tOverwriten byte: %i\n"
                        "\tPointer: %p\n",
                        mem_info->line, mem_info->file, line, file,
                        (int)mem_info->post_buf[i], (int)p[i], mem_info->bytes,
                        mem_info->bytes + i, ptr);

            err = -1;
        }
    }

    remove_ptr_info(ptr_info);
    FREE(ptr_info);

    return err;
}

void
free_d(void *ptr, int line, const char *file)
{
    do_free_d(ptr, line, file);
}

#endif

#ifdef XMALLOC

#define XMALLOC_ERR_MSG "Out of memory for xmalloc.\nAborting now.\n"
#define XCALLOC_ERR_MSG "Out of memory for xcalloc.\nAborting now.\n"
#define XREALLOC_ERR_MSG "Out of memory for xrealloc.\nAborting now.\n"
#define XFREE_ERR_MSG "Unable to free memory in xfree.\nAborting now.\n"

void *
xmalloc(size_t n)
{
    void *ptr;

    ASSUME(n > 0);

    ptr = MALLOC(n);
    if (ERR(ptr == NULL)) {
        fputs(XMALLOC_ERR_MSG, stderr);
        abort();
    }

    return ptr;
}

void *
xcalloc(size_t n, size_t size)
{
    void *ptr;

    ASSUME(n > 0);
    ASSUME(size > 0);

    ptr = CALLOC(n, size);
    if (ERR(ptr == NULL)) {
        fputs(XCALLOC_ERR_MSG, stderr);

        abort();

        ASSUME_UNREACHABLE();
    }

    return ptr;
}

void *
xrealloc(void *ptr, size_t n)
{
    ASSUME(n > 0);

    ptr = REALLOC(ptr, n);
    if (ERR(ptr == NULL)) {
        fputs(XREALLOC_ERR_MSG, stderr);
        abort();
    }

    return ptr;
}

#ifndef NDEBUG

void *
xmalloc_d(size_t n, int line, const char *file)
{
    void *ptr;

    ASSUME(line >= 0);
    ASSUME(file != NULL);

    if (ERR(n == 0)) {
        fprintf(stderr, "Cannot allocate 0 bytes in xmalloc.\n\tLine: %i\n"
                "\tFile: %s\nAborting now.\n",
                line, file);

        abort();

        ASSUME_UNREACHABLE();
    }

    ASSUME(n != 0);

    ptr = malloc_d(n, line, file);
    if (ERR(ptr == NULL)) {
        fputs(XMALLOC_ERR_MSG, stderr);

        abort();

        ASSUME_UNREACHABLE();
    }

    return ptr;
}

void *
xcalloc_d(size_t n, size_t size, int line, const char *file)
{
    void *ptr;

    ASSUME(line >= 0);
    ASSUME(file != NULL);

    if (ERR(n == 0) || ERR(size == 0)) {
        fprintf(stderr, "Cannot allocate 0 bytes in xcalloc.\n\tLine: %i\n"
                "\tFile: %s\nAborting now.\n",
                line, file);

        abort();

        ASSUME_UNREACHABLE();
    }

    ASSUME(n > 0);
    ASSUME(size > 0);

    ptr = calloc_d(n, size, line, file);
    if (ERR(ptr == NULL)) {
        fputs(XCALLOC_ERR_MSG, stderr);

        abort();

        ASSUME_UNREACHABLE();
    }

    return ptr;
}

void *
xrealloc_d(void *ptr, size_t n, int line, const char *file)
{
    ASSUME(line >= 0);
    ASSUME(file != NULL);

    if (ERR(n == 0)) {
        fprintf(stderr, "Cannot allocate 0 bytes in xrealloc.\n\tLine: %i\n"
                "\tFile: %s\nAborting now.\n",
                line, file);

        abort();

        ASSUME_UNREACHABLE();
    }

    ASSUME(n > 0);

    ptr = realloc_d(ptr, n, line, file);
    if (ERR(ptr == NULL)) {
        fputs(XREALLOC_ERR_MSG, stderr);

        abort();

        ASSUME_UNREACHABLE();
    }

    return ptr;
}

void
xfree_d(void *ptr, int line, const char *file)
{
    if (ERR(do_free_d(ptr, line, file) != 0)) {
        fputs(XFREE_ERR_MSG, stderr);

        abort();

        ASSUME_UNREACHABLE();
    }
}

#endif

#endif
