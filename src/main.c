#include "main.h"

#include "alloc.h"

#include <assert.h>
#include <stdio.h>

#ifdef NDEBUG

#include <stdlib.h>
#include <time.h>

#endif

int main(int argc, char *argv[])
{
    ASSUME(argc > 0);

#ifdef NDEBUG

    srand(time(NULL));

#endif

    printf("Hello world!\n");

    return 0;
}
