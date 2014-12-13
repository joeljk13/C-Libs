#include "../src/main.h"
#include "../src/alloc.h"

#include "test.h"

#include <stdio.h>

int
main(void)
{
    void *ptr;

    (void)TEST_FAIL;

    TEST_CHECK("alloc_init()");
    alloc_init();
    TEST_PASS();

    TEST_CHECK("alloc_size()");
    alloc_size(1);
    TEST_PASS();

    TEST_CHECK("jxmalloc()");
    ptr = jxmalloc(1);
    TEST_PASS();

    TEST_CHECK("jxfree()");
    jxfree(ptr);
    TEST_PASS();

    TEST_CHECK("jxcalloc()");
    ptr = jxcalloc(1, 1);
    TEST_PASS();

    TEST_CHECK("jxrealloc()");
    ptr = jxrealloc(ptr, 1);
    TEST_PASS();

    TEST_CHECK("jxfree()");
    jxfree(ptr);
    TEST_PASS();

    TEST_CHECK("jmalloc()");
    ptr = jmalloc(1);
    TEST_PASS();

    TEST_CHECK("jfree()");
    jfree(ptr);
    TEST_PASS();

    TEST_CHECK("jcalloc()");
    ptr = jcalloc(1, 1);
    TEST_PASS();

    TEST_CHECK("jrealloc()");
    ptr = jrealloc(ptr, 1);
    TEST_PASS();

    TEST_CHECK("jfree()");
    jfree(ptr);
    TEST_PASS();

    TEST_CHECK("alloc_free()");
    alloc_free();
    TEST_PASS();

    TEST_TODO(Implement alloc tests);

    return 0;
}
