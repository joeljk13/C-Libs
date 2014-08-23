#ifndef MAIN_H_
#define MAIN_H_ 1

#include <assert.h>

/* Swaps a and b, which are of type T */
#define SWAP(a,b,T) do { \
    T tmp_ = (b); \
    (b) = (a); \
    (a) = tmp_; \
} while (0)

/* assert(IMPLIES(a,b)) asserts that a implies b */
#define IMPLIES(a,b) (!(a) || (b))

/* A simple assertion with a message */
#define ASSERT(b,m) assert(((m), (b)))

/* Tells the compiler to assume b. Useful for optimization. In debug mode,
 * automatically asserts that b is true */
#define ASSUME(b) do { \
    ASSERT(b, "assertion failed in ASSUME"); \
    if (!(b)) { \
        __builtin_unreachable(); \
    } \
} while (0)

/* Checks whether p is a null pointer for the purposes of error checking */
#define IS_NULLPTR(p) (__builtin_expect((p) == NULL, 0))

/* Allows for TODO messages in code, and fails if reached in execution during
 * debug mode */
#define TODO(m) ASSERT(0, "TODO - " #m)

#endif
