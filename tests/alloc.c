#include "../src/main.h"
#include "../src/alloc.h"
#include "test.h"

#include <stdio.h>

int
main(void)
{
    void *ptr;

    alloc_init();

    alloc_size(1);

    ptr = jxmalloc(1);
    jxfree(ptr);

    ptr = jxcalloc(1, 1);

    ptr = jxrealloc(ptr, 1);

    jxfree(ptr);

    ptr = jmalloc(1);

    jfree(ptr);

    ptr = jcalloc(1, 1);

    ptr = jrealloc(ptr, 1);

    jfree(ptr);

    alloc_free();

    return 0;
}
