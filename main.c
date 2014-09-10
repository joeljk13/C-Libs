#include "main.h"

#include "alloc.h"
#include "args.h"


int main(int argc, char *argv[])
{
    char *ptr;

    alloc_init();
    if (args_init(argc, argv, "") != 0) {
        alloc_free();
    }

    ptr = MALLOC(1);
    ptr[1] = 20;
    FREE(ptr);

    args_free();
    alloc_free();
    return 0;
}
