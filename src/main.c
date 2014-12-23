#include "main.h"

#include <stdio.h>

int
main(int argc, char *argv[])
{
    ASSUME(argc > 0);
    ASSUME(argv[argc] == NULL);

    printf("Hello world!\n");

    return 0;
}
