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

    ASSERT(alloc_min_buf_size != 0, "alloc_min_buf_size should not be 0");
    ASSERT((alloc_min_buf_size & (alloc_min_buf_size - 1)) == 0,
           "alloc_min_buf_size should be a power of 2");
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
static void
mem_fail(size_t bytes, int line, const char *file)
{
    ASSUME(line >= 0);
    ASSUME(file != NULL);

    fprintf(stderr, "Memory failure!\n\tLine: %i\n\tFile: %s\n\tBytes: %zu\n",
            line, file, bytes);
}

static int alloc_freed = 0;

// TODO - Optimize the pointer info storage. Maybe adapt ptrvec to a vector of
// struct ptr_info? Or maybe use a tree to get O(log(n)) insertion, removal,
// and deletion?

struct ptr_info {
    const void *ptr;
    size_t bytes;
};

static struct ptr_info *ptr_infos = NULL;
static size_t n_ptr_infos = 0;
static size_t cap_ptr_infos = 0;

static inline int
add_ptr_info(const void *ptr, size_t bytes)
{
    struct ptr_info *tmp;

    ASSUME(ptr != NULL);
    ASSUME(bytes > 0);

    if (n_ptr_infos == cap_ptr_infos) {
        size_t cap;

        cap = cap_ptr_infos == 0 ? 1 : cap_ptr_infos * 2;

        tmp = REALLOC(ptr_infos, cap * sizeof(*ptr_infos));
        if (ERR(tmp == NULL)) {
            cap = cap_ptr_infos + 1;

            tmp = REALLOC(ptr_infos, cap * sizeof(*ptr_infos));
            if (ERR(tmp == NULL)) {
                mem_fail(cap * sizeof(*ptr_infos), __LINE__, __FILE__);
                return -1;
            }
        }
        ptr_infos = tmp;

        cap_ptr_infos = cap;
    }

    ptr_infos[n_ptr_infos].ptr = ptr;
    ptr_infos[n_ptr_infos].bytes = bytes;
    ++n_ptr_infos;

    return 0;
}

static inline const void *
find_ptr_info(const void *ptr)
{
    ASSUME(ptr != NULL);

    for (size_t i = 0; i < n_ptr_infos; ++i) {
        for (const char *p = ptr_infos[i].ptr, *p_end = p + ptr_infos[i].bytes;
             p != p_end; ++p) {

            if (p == ptr) {
                return ptr_infos[i].ptr;
            }
        }
    }

    // Pointer not found
    return NULL;
}

static inline int
remove_ptr_info(const void *ptr)
{
    ASSUME(ptr != NULL);

    for (size_t i = 0; i < n_ptr_infos; ++i) {
        if (ptr != ptr_infos[i].ptr) {
            continue;
        }

        // The order doesn't really matter, so just replace this with the
        // last ptr_info and remove the last
        ptr_infos[i] = ptr_infos[--n_ptr_infos];

        if (cap_ptr_infos > 2 * (n_ptr_infos + 1)) {
            struct ptr_info *tmp;

            tmp = REALLOC(ptr_infos, n_ptr_infos * sizeof(*ptr_infos));
            if (ERR(tmp == NULL)) {
                mem_fail(n_ptr_infos * sizeof(*ptr_infos), __LINE__, __FILE__);
                return -1;
            }
            ptr_infos = tmp;

            cap_ptr_infos = n_ptr_infos;
        }

        return 0;
    }

    ASSUME_UNREACHABLE();

    return -1;
}

static void
alloc_exit(void)
{
    if (!alloc_freed) {
        alloc_freed = 1;
        alloc_free();
    }
}

int
alloc_init(void)
{
    return atexit(&alloc_exit);
}

int
alloc_free(void)
{
    alloc_freed = 1;

    for (size_t i = 0; i < n_ptr_infos; ++i) {
        const struct mem_info *mem_info =
            (const struct mem_info *)ptr_infos[i].ptr;

        fprintf(stderr, "Memory not freed!\n\tLine: %i\n\tFile: %s\n"
                "\tBytes: %zu\n\tPointer: %p\n",
                mem_info->line, mem_info->file, mem_info->bytes,
                (const void *)mem_info);

        FREE(ptr_infos[i].ptr);
    }

    FREE(ptr_infos);

    return n_ptr_infos != 0;
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
            str[i] = (char)(rand() % ((int)CHAR_MAX - (int)CHAR_MIN) +
                            CHAR_MIN);
        } while (str[i] == '\0');

        ASSERT(str[i] != '\0',
               "str cannot have a null char in the middle of it");
    }

    str[len] = '\0';

    return str;
}

static void *
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

    // TODO - check for overflow in n * size; these assertions are a start but
    // not reliable
    ASSERT(n <= n * size, "overflow has occcured");
    ASSERT(size <= n * size, "overflow has occured");

    return alloc_d(n * size, 1, line, file);
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

    if (ERR(n == 0)) {
        fprintf(stderr, "Warning: realloc(ptr, 0) is not portable, since it\n"
                "may or may not free ptr. Here ptr is being freed.\n"
                "\tLine: %i\n\tFile: %s\n\tPointer: %p\n",
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

    mem_info = (const struct mem_info *)old_ptr;

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
                "\tBytes: %zu\n"
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
                    "\tBytes: %zu\n\tOverwriten byte: %zu\n\tPointer: %p\n",
                    mem_info->line, mem_info->file, line, file,
                    (int)mem_info->pre_buf[i], (int)p[i], mem_info->bytes,
                    i - (size_t)pre_len, ptr);

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
                        "\tBytes: %zu\n\tOverwriten byte: %zu\n"
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
