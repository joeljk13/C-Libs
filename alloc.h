#ifndef ALLOC_H_
#define ALLOC_H_ 1

#include "main.h"

#include <stdlib.h>

#ifdef JEMALLOC
#include <jemalloc/jemalloc.h>
#endif

/* r ## alloc functions allocate just like their counterparts, but somewhere in
 * the range [nmin, *nmax]. They try to allocate larger values, but fall back
 * on lower values on failure. *nmax is set to the actual amount allocated. If not
 * even nmin could be allocated, they return NULL. */

void *
rmalloc(size_t nmin, size_t *nmax) MALLOC_LIKE NONNULL;

void *
rcalloc(size_t nmin, size_t *nmax, size_t size) MALLOC_LIKE NONNULL;

void *
rrealloc(void *ptr, size_t nmin, size_t *nmax) NONNULL_AT(3);

#ifdef NDEBUG

#define jrmalloc(n,x) rmalloc((n), (x));
#define jrcalloc(n,x,s) rcalloc((n), (x), (s));
#define jrrealloc(p,n,x) rrealloc((p), (n), (x));

#else

void *
rmalloc_d(size_t nmin, size_t *nmax, int line, const char *file) MALLOC_LIKE
    NONNULL;

void *
rcalloc_d(size_t nmin, size_t *nmax, size_t size, int line, const char *file)
    MALLOC_LIKE NONNULL;

void *
rrealloc_d(void *ptr, size_t nmin, size_t *nmax, int line, const char *file)
    NONNULL_AT(3, 5);

#define jrmalloc(n,x) rmalloc_d((n), (x), __LINE__, __FILE__)
#define jrcalloc(n,x,s) rcalloc_d((n), (x), (s), __LINE__, __FILE__)
#define jrrealloc(p,n,x) rrealloc_d((p), (n), (x), __LINE__, __FILE__)

#endif

#ifdef NDEBUG

#define alloc_size(s) ((void)0)
#define alloc_init() ((void)0)
#define alloc_free() ((void)0)

#ifdef JEMALLOC

#define jmalloc(n) jemalloc((n))
#define jcalloc(n,s) jecalloc((n), (s))
#define jrealloc(p,s) jerealloc((p), (s))
#define jfree(p) jefree((void *)(p))

#else

#define jmalloc(n) malloc((n))
#define jcalloc(n,s) calloc((n), (s))
#define jrealloc(p,s) realloc((p), (s))
#define jfree(p) free((void *)(p))

#endif

#else

void
alloc_size(size_t size);

#define alloc_init() ((void)0)

void
alloc_free(void);

void *
alloc_d(size_t n, int line, const char *file);

void *
calloc_d(size_t n, size_t size, int line, const char *file);

void *
realloc_d(void *ptr, size_t n, int line, const char *file);

void
free_d(const void *ptr, int line, const char *file);

#define jmalloc(n) malloc_d((n), __LINE__, __FILE__)
#define jcalloc(n,s) calloc_d((n), (s), __LINE__, __FILE__)
#define jrealloc(p,s) realloc_d((p), (s), __LINE__, __FILE__)
#define jfree(p) free_d((void *)(p), __LINE__, __FILE__)

#endif

#ifdef XMALLOC

/* These functions are the same as the above functions, except that on failure,
 * they print a message to stderr and call abort(). */

void *
xmalloc(size_t n) MALLOC_LIKE RETURNS_NONNULL;

void *
xcalloc(size_t n, size_t size) MALLOC_LIKE RETURNS_NONNULL;

void *
xrealloc(void *ptr, size_t n) RETURNS_NONNULL;

void *
xrmalloc(size_t nmin, size_t *nmax) MALLOC_LIKE NONNULL;

void *
xrcalloc(size_t nmin, size_t *nmax, size_t size) MALLOC_LIKE NONNULL;

void *
xrrealloc(void *ptr, size_t nmin, size_t *nmax) NONNULL_AT(3);

#ifdef NDEBUG

#define jxmalloc(n) xmalloc((n))
#define jxcalloc(n,s) xcalloc((n), (s))
#define jxrealloc(p,n) xrealloc((p), (n))

#define jxrmalloc(n,x) xrmalloc((n), (x));
#define jxrcalloc(n,x,s) xrcalloc((n), (x), (s));
#define jxrrealloc(p,n,x) xrrealloc((p), (n), (x));

#else

void *
xmalloc_d(size_t n, int line, const char *file) MALLOC_LIKE RETURNS_NONNULL
    NONNULL;

void *
xcalloc_d(size_t n, size_t size, int line, const char *file) MALLOC_LIKE
    RETURNS_NONNULL NONNULL;

void *
xrealloc_d(void *ptr, size_t n, int line, const char *file) RETURNS_NONNULL
    NONNULL_AT(4);

void *
rmalloc_d(size_t nmin, size_t *nmax, int line, const char *file) MALLOC_LIKE
    NONNULL;

void *
rcalloc_d(size_t nmin, size_t *nmax, size_t size, int line, const char *file)
    MALLOC_LIKE NONNULL;

void *
rrealloc_d(void *ptr, size_t nmin, size_t *nmax, int line, const char *file)
    NONNULL_AT(3, 5);

#define jxmalloc(n) xmalloc_d((n), __LINE__, __FILE__)
#define jxcalloc(n,s) xcalloc_d((n), (s), __LINE__, __FILE__)
#define jxrealloc(p,n) xrealloc_d((p), (n), __LINE__, __FILE__)

#define jrmalloc(n,x) xrmalloc_d((n), (x), __LINE__, __FILE__)
#define jrcalloc(n,x,s) xrcalloc_d((n), (x), (s), __LINE__, __FILE__)
#define jrrealloc(p,n,x) xrrealloc_d((p), (n), (x), __LINE__, __FILE__)

#endif

#endif
