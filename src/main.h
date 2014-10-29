#ifndef MAIN_H_
#define MAIN_H_ 1

// #define JEMALLOC

#include <assert.h>
#include <stddef.h>

/* This file contains GCC specific code. */

#define NONNULL __attribute__((nonnull))
#define NONNULL_AT(...) __attribute__((nonnull(__VA_ARGS__)))

#ifdef __GNUC__

#define RETURNS_NONNULL __attribute__((returns_nonnull))

#endif

#define CONST __attribute__((const))

#define PURE __attribute__((pure))

#define MALLOC_LIKE __attribute__((malloc))

#define UNUSED(v) ((void)(v))

/* Says that a implies b. Designed to be used with ASSERT, ASSUME, etc. */
#define IMPLIES(a,b) (!(a) || (b))

/* A simple assertion with a message. */
#define ASSERT(b,m) assert((b) && (m))

/* A compile time assertion with a message. */
#define STATIC_ASSERT(b,m) do { \
    typedef char STATIC_ASSERT_[-1+2*!!((b) && (m))]; \
} while (0)

#ifdef NDEBUG

/* Tells the compiler that code is unreachable, for optimization. In debug
 * mode, raises an assertion if the code is reached. */
#define ASSUME_UNREACHABLE() __builtin_unreachable()

/* Tells the compiler to assume b, for optimization. In debug mode, raises an
 * assertion if b is 0. */
#define ASSUME(b) do { \
    if (!(b)) { \
        __builtin_unreachable(); \
    } \
} while (0)

/* Tells the compiler that b is unlikely or likely to be 0, respectively, for
 * optimization. In debug mode, these don't do anything for optimization to
 * make debugging easier. */
#define LIKELY(b) __builtin_expect(!!(b), 1)
#define UNLIKELY(b) __builtin_expect(!!(b), 0)

#else

#define ASSUME_UNREACHABLE() do { \
    ASSERT(0, "assertion failed in ASSUME_UNREACHABLE"); \
} while (0)

#define ASSUME(b) do { \
    ASSERT((b), "assertion failed in ASSUME"); \
} while (0)

#define LIKELY(b) (!!(b))
#define UNLIKELY(b) (!!(b))

#endif

/* Wrap this around the test when checking for an error condition. This is for
 * self-documenting error checking, and for optimization when not in debug
 * mode. */
#define ERR(b) UNLIKELY(b)

#define TODO_(m) ASSERT(0, #m)

/* Allows for TODO messages in code, and fails if reached in execution during
 * debug mode. Works best when m is not a string, but just text (i.e. use
 * TODO(text) rather than TODO("text")). */
#define TODO(m) TODO_(TODO - m)

#endif
