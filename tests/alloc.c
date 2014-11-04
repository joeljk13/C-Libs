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
    pass();

    check("alloc_size()");
    alloc_size(1);
    pass();

    check("jxmalloc()");
    ptr = jxmalloc(1);
    pass();

    check("jxfree()");
    jxfree(ptr);
    pass();

    check("jxcalloc()");
    ptr = jxcalloc(1, 1);
    pass();

    check("jxrealloc()");
    ptr = jxrealloc(ptr, 1);
    pass();

    check("jxfree()");
    jxfree(ptr);
    pass();

    check("jmalloc()");
    ptr = jmalloc(1);
    pass();

    check("jfree()");
    jfree(ptr);
    pass();

    check("jcalloc()");
    ptr = jcalloc(1, 1);
    pass();

    check("jrealloc()");
    ptr = jrealloc(ptr, 1);
    pass();

    check("jfree()");
    jfree(ptr);
    pass();

    check("alloc_free()");
    alloc_free();
    pass();

    return 0;
}
