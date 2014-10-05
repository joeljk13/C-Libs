#ifndef ALLOC_H_
#define ALLOC_H_ 1

#include "main.h"

#include <stdlib.h>

#ifndef NDEBUG

/* Tells the allocator to have a buffer size of at least size bytes to allow
 * for upcoming allocations of large data types. */
void
alloc_size(size_t size);

/* Initializes the allocator. This should be called before any allocations. */
#define alloc_init() ((void)0)

/* All of the following functions may print messages to stderr. */

/* Frees the allocator and all unfreed memory. */
void
alloc_free(void);

/* The debug equivalent of malloc(). MALLOC is defined to call this when
 * debugging. */
void *
malloc_d(size_t n, int line, const char *file);

/* The debug equivalent of calloc(). CALLOC is defined to call this when
 * debugging. */
void *
calloc_d(size_t n, size_t size, int line, const char *file);

/* The debug equivalent of realloc(). REALLOC is defined to call this when
 * debugging. */
void *
realloc_d(void *ptr, size_t n, int line, const char *file);

/* The debug equivalent of free(). FREE is defined to call this when
 * debugging. */
void
free_d(const void *ptr, int line, const char *file);

#define MALLOC(n) malloc_d((n), __LINE__, __FILE__)
#define CALLOC(n,s) calloc_d((n), (s), __LINE__, __FILE__)
#define REALLOC(p,s) realloc_d((p), (s), __LINE__, __FILE__)
#define FREE(p) free_d((void *)(p), __LINE__, __FILE__)

#else

#define alloc_size(s) ((void)0)

#define alloc_init() ((void)0)
#define alloc_free() ((void)0)

#define MALLOC(n) malloc(n)
#define CALLOC(n,s) calloc((n), (s))
#define REALLOC(p,s) realloc((p), (s))
#define FREE(p) free((void *)(p))

#endif

#endif
