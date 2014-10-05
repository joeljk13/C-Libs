#ifndef MAIN_H_
#define MAIN_H_ 1

#include <assert.h>
#include <stddef.h>

/* Says that a implies b. Can be used with ASSERT or ASSUME. */
#define IMPLIES(a,b) (!(a) || (b))

/* A simple assertion with a message. */
#define ASSERT(b,m) assert(((m), (b)))

#define STATIC_ASSERT(b,m) do { \
    typedef char STATIC_ASSERT_[!!(b) * 2 - 1]; \
} while (0)

#ifdef NDEBUG

#define ASSUME_UNREACHABLE() __builtin_unreachable()

/* Tells the compiler to assume b. Useful for optimization. In debug mode,
 * asserts that b is true. */
#define ASSUME(b) do { \
    if (!(b)) { \
        __builtin_unreachable(); \
    } \
} while (0)

#define LIKELY(b) __builtin_expect(!!(b), 1)
#define UNLIKELY(b) __builtin_expect(!!(b), 0)

#else

#define ASSUME_UNREACHABLE() do { \
    ASSERT(0, "assertion failed in ASSUME_UNREACHABLE"); \
} while (0)

#define ASSUME(b) do { \
    ASSERT(b, "assertion failed in ASSUME"); \
} while (0)

#define LIKELY(b) (!!(b))
#define UNLIKELY(b) (!!(b))

#endif

/* Wrap this around the test when checking for an error condition */
#define ERR(b) UNLIKELY(b)

#define TODO_(m) ASSERT(0, #m)

/* Allows for TODO messages in code, and fails if reached in execution during
 * debug mode. Works best when m is not a string, but just text (e.g. use
 * TODO(test) rather than TODO("test")). */
#define TODO(m) TODO_(TODO - m)

#endif
