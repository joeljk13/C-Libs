#include "main.h"
#include "alloc.h"

int main(int argc, char *argv[])
{
    char *ptr;

#ifdef NDEBUG
    printf("Not in debug mode!\n");
    return 0;
#endif

    alloc_init();

    alloc_free();
    return 0;
}
