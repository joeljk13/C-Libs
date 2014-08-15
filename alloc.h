#ifndef ALLOC_H_
#define ALLOC_H_ 1

#include <stdlib.h>

#ifndef NDEBUG

void
init_alloc(void);

void
free_alloc(void);

void *
malloc_d(size_t n, int line, const char *file);

void *
calloc_d(size_t n, size_t size, int line, const char *file);

void *
realloc_d(void *ptr, size_t n, int line, const char *file);

void
free_d(void *ptr, int line, const char *file);

#define MALLOC(n) malloc_d((n), __LINE__, __FILE__)
#define CALLOC(n,s) calloc_d((n), (s), __LINE__, __FILE__)
#define REALLOC(p,s) realloc_d((p), (s), __LINE__, __FILE__)
#define FREE(p) free_d(p, __LINE__, __FILE__)

#else

#define init_alloc()
#define free_alloc()

#define MALLOC(n) malloc(n)
#define CALLOC(n,s) calloc((n), (s))
#define REALLOC(p,s) realloc((p), (s))
#define FREE(p) free(p)

#endif

#endif
