#include "../src/main.h"
#include "../src/alloc.h"

int
main(void)
{
    void *ptr;

    ptr = jmalloc(1);
    jfree(ptr);

    return 0;
}
