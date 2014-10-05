#include "main.h"

#include "alloc.h"
#include "stack.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    TODO(yeah);

    char *ptr;
    struct stack stack;
    int a = 1;

    alloc_init();

    stack_init(&stack);
    assert(stack_is_empty(&stack));
    stack_free(&stack);

    stack_init(&stack);
    stack_push(&stack, &a);
    assert(!stack_is_empty(&stack));
    assert(stack_peek(&stack) == &a);
    assert(stack_pop(&stack) == &a);
    assert(stack_is_empty(&stack));
    stack_push(&stack, &a);
    stack_free(&stack);

    alloc_free();

    return 0;
}
