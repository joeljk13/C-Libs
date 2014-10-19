#include "../src/main.h"
#include "../src/alloc.h"
#include "test.h"

#include <stdio.h>

int
main(void)
{
    void *ptr;

    alloc_init();

    puts("A");

    // alloc_size(48);

    puts("B");

    ptr = jxmalloc(1);
    puts("C");
    jxfree(ptr);

    puts("D");

    ptr = jxcalloc(1, 1);

    puts("E");

    // Doesn't work without this
    // ptr = NULL;

    ptr = jxrealloc(ptr, 1);

    puts("F");

    jxfree(ptr);

    puts("G");

    ptr = jmalloc(1);

    puts("H");

    jfree(ptr);

    puts("I");

    ptr = jcalloc(1, 1);

    puts("J");

    ptr = jrealloc(ptr, 1);

    puts("K");

    jfree(ptr);

    puts("# TODO - implement alloc tests");

    alloc_free();

    return 0;
}
