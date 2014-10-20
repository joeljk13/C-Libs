#include "../src/main.h"
#include "../src/alloc.h"
#include "test.h"

#include <stdio.h>

int
main(void)
{
    void *ptr;

    alloc_init();

    fprintf(stderr, "A\n");

    alloc_size(1);

    fprintf(stderr, "B\n");

    ptr = jxmalloc(1);
    fprintf(stderr, "C\n");
    jxfree(ptr);

    fprintf(stderr, "D\n");

    ptr = jxcalloc(1, 1);

    fprintf(stderr, "E\n");

    ptr = jxrealloc(ptr, 1);

    fprintf(stderr, "F\n");

    jxfree(ptr);

    fprintf(stderr, "G\n");

    ptr = jmalloc(1);

    fprintf(stderr, "H\n");

    jfree(ptr);

    fprintf(stderr, "I\n");

    ptr = jcalloc(1, 1);

    fprintf(stderr, "J\n");

    ptr = jrealloc(ptr, 1);

    fprintf(stderr, "K\n");

    jfree(ptr);

    fprintf(stderr, "# TODO - implement alloc tests\n");

    alloc_free();

    return 0;
}
