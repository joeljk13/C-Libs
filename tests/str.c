#include "../src/main.h"
#include "../src/str.h"

#include "../src/alloc.h"

#include "test.h"

int
main(void)
{
    struct str str;

    alloc_init();

    TEST_CHECK("str_init()");

    str_init(&str);

    TEST_ASSERT(IMPLIES(str.ptr == NULL, str.length == 0 &&
                        str.capacity == 0));

    TEST_PASS();

    str_free(&str);

    TEST_TODO(Implement str tests);

    alloc_free();

    return 0;
}

