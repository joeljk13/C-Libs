#ifndef ALLOC_H_
#define ALLOC_H_ 1

#include "main.h"

#include <stdlib.h>

#ifdef JEMALLOC
#include <jemalloc/jemalloc.h>
#endif

#ifndef NDEBUG

#define XMALLOC

#endif

#ifdef NDEBUG

#define alloc_size(s) ((void)0)
#define alloc_init() ((void)0)
#define alloc_free() (0)

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

/* Increases the buffer to be at least size bytes. */
void
alloc_size(size_t size);

#define alloc_init() ((void)0)

int
alloc_free(void);

void *
malloc_d(size_t n, int line, const char *file);

void *
calloc_d(size_t n, size_t size, int line, const char *file);

void *
realloc_d(void *ptr, size_t n, int line, const char *file);

void
free_d(void *ptr, int line, const char *file);

#define jmalloc(n) malloc_d((n), __LINE__, __FILE__)
#define jcalloc(n,s) calloc_d((n), (s), __LINE__, __FILE__)
#define jrealloc(p,s) realloc_d((p), (s), __LINE__, __FILE__)
#define jfree(p) free_d((void *)(p), __LINE__, __FILE__)

#endif

#ifdef XMALLOC

/* These functions are the same as the above functions, except that on failure,
 * they print a message to stderr and call abort(). Also, these functions
 * assume that you don't try to allocate 0 bytes. */

void *
xmalloc(size_t n);

void *
xcalloc(size_t n, size_t size);

void *
xrealloc(void *ptr, size_t n);

#ifdef NDEBUG

#define jxmalloc(n) xmalloc((n))
#define jxcalloc(n,s) xcalloc((n), (s))
#define jxrealloc(p,n) xrealloc((p), (n))
#define jxfree(p) jfree((void *)p)

#else

void *
xmalloc_d(size_t n, int line, const char *file);

void *
xcalloc_d(size_t n, size_t size, int line, const char *file);

void *
xrealloc_d(void *ptr, size_t n, int line, const char *file);

void
xfree_d(void *ptr, int line, const char *file);

#define jxmalloc(n) xmalloc_d((n), __LINE__, __FILE__)
#define jxcalloc(n,s) xcalloc_d((n), (s), __LINE__, __FILE__)
#define jxrealloc(p,n) xrealloc_d((p), (n), __LINE__, __FILE__)
#define jxfree(p) xfree_d((p), __LINE__, __FILE__)

#endif

#endif

#endif
