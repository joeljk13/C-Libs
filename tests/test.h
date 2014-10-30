#ifndef TEST_H_
#define TEST_H_ 1

#include "../src/main.h"

#include <stdio.h>

static inline void
check(const char *str)
{
    ASSUME(str != NULL);

    printf("%sChecking %s... ", COLOR_RESET, str);
}

static inline void
ok(void)
{
    printf("%s[OK]%s\n", COLOR_GREEN, COLOR_RESET);
}

static inline void
fail(void)
{
    printf("%s[FAIL]%s\n", COLOR_RED, COLOR_RESET);
}

#endif
