#ifndef ALLOC_H_
#define ALLOC_H_ 1

#include "main.h"

#include <stdlib.h>

#ifndef NDEBUG

void
alloc_size(size_t size);

// There's nothing to initialize!
#define alloc_init() ((void)0)

void
alloc_free(void);

void *
malloc_d(size_t n, int line, const char *file);

void *
calloc_d(size_t n, size_t size, int line, const char *file);

void *
realloc_d(void *ptr, size_t n, int line, const char *file);

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
