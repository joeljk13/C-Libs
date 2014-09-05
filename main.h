#ifndef MAIN_H_
#define MAIN_H_ 1

#include <assert.h>

/* Says that a implies b. Can be used with ASSERT or ASSUME */
#define IMPLIES(a,b) (!(a) || (b))

/* A simple assertion with a message */
#define ASSERT(b,m) assert(((m), (b)))

#define ASSUME_UNREACHABLE() (__builtin_unreachable())

/* Tells the compiler to assume b. Useful for optimization. In debug mode,
 * automatically asserts that b is true */
#define ASSUME(b) do { \
    ASSERT(b, "assertion failed in ASSUME"); \
    if (!(b)) { \
        ASSUME_UNREACHABLE(); \
    } \
} while (0)

/* Checks whether p is a null pointer for the purposes of error checking */
#define IS_NULLPTR(p) (__builtin_expect((p) == NULL, 0))

/* Allows for TODO messages in code, and fails if reached in execution during
 * debug mode */
#define TODO(m) ASSERT(0, TODO - m)

#endif
