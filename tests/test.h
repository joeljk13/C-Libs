#ifndef TEST_H_
#define TEST_H_ 1

#include "../src/main.h"

#include <stdio.h>

static inline void
check(const char *str)
{
    ASSUME(str != NULL);

    printf("%sChecking %s...%s ", COLOR_RESET, str, COLOR_RED);
}

static inline void
pass(void)
{
    printf("%s[PASS]%s\n", COLOR_GREEN, COLOR_RED);
}

static inline void
fail(void)
{
    printf("%s[FAIL]\n", COLOR_RED);
}

#endif
