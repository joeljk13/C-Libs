#include "../src/main.h"
#include "../src/alloc.h"
#include "test.h"

#include <stdio.h>

int
main(void)
{
    void *ptr;

    check("alloc_init()");
    alloc_init();
    ok();

    check("alloc_size()");
    alloc_size(1);
    ok();

    check("jxmalloc()");
    ptr = jxmalloc(1);
    ok();

    check("jxfree()");
    jxfree(ptr);
    ok();

    check("jxcalloc()");
    ptr = jxcalloc(1, 1);
    ok();

    check("jxrealloc()");
    ptr = jxrealloc(ptr, 1);
    ok();

    check("jxfree()");
    jxfree(ptr);
    ok();

    check("jmalloc()");
    ptr = jmalloc(1);
    ok();

    check("jfree()");
    jfree(ptr);
    ok();

    check("jcalloc()");
    ptr = jcalloc(1, 1);
    ok();

    check("jrealloc()");
    ptr = jrealloc(ptr, 1);
    ok();

    check("jfree()");
    jfree(ptr);
    ok();

    check("alloc_free()");
    alloc_free();
    ok();

    return 0;
}
