#ifndef TEST_H_
#define TEST_H_ 1

#include "../src/main.h"

#include <stdio.h>

static void
TEST_CHECK(const char *str)
{
    ASSUME(str != NULL);

    printf("%sChecking %s... %s", COLOR_RESET, str, COLOR_RED);
    fflush(stdout);
}

static void
TEST_PASS(void)
{
    printf("%s[PASS]\n%s", COLOR_GREEN, COLOR_RED);
    fflush(stdout);
}

static void
TEST_FAIL(void)
{
    printf("%s[FAIL]\n", COLOR_RED);
    fflush(stdout);
}

static void
TEST_TODO_(const char *msg)
{
    printf("%s[TODO] %s\n%s", COLOR_YELLOW, msg, COLOR_RED);
}

#define TEST_ASSERT(b) do { \
    if (!(b)) { \
        TEST_FAIL(); \
        return 1; \
    } \
} while (0)

#define TEST_TODO(m) do { \
    TEST_TODO_(#m); \
} while (0)

#endif
