#include "main.h"
i
#include "alloc.h"
#include "args.h"


int main(int argc, char *argv[])
{
    char *ptr;

    alloc_init();
    args_init(argc, argv);

    ptr = MALLOC(1);
    ptr[1] = 20;
    FREE(ptr);

    args_free();
    alloc_free();
    return 0;
}
