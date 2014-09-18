#include "stack.h"
#include "main.h"

#include "alloc.h"

struct stack_elem {
    struct stack_elem *prev;
    void *data;
};

void
stack_init(struct stack *stack)
{
    ASSUME(stack != NULL);
    
    stack->bottom = NULL;
}

void
stack_free(struct stack *stack)
{
    struct stack_elem *prev;

    ASSUME(stack != NULL);

    if (stack->bottom == NULL) {
        return;
    }

    prev = stack->bottom->prev;
    FREE(stack->bottom);

    while (prev != NULL) {
        stack->bottom = prev;
        prev = prev->prev;
        FREE(stack->bottom);
    }
}

int
stack_push(struct stack *stack, void *data)
{
    struct stack_elem *elem;

    ASSUME(stack != NULL);

    elem = MALLOC(sizeof(*elem));
    if (ERR(elem == NULL)) {
        return -1;
    }

    elem->data = data;
    elem->prev = stack->bottom;
    stack->bottom = elem;

    return 0;
}

void *
stack_pop(struct stack *stack)
{
    struct stack_elem *elem;
    void *data;

    ASSUME(stack != NULL);
    ASSUME(!stack_is_empty(stack));

    elem = stack->bottom;
    stack->bottom = elem->prev;

    data = elem->data;

    FREE(elem);

    return data;
}

void *
stack_peek(struct stack *stack)
{
    ASSUME(stack != NULL);
    ASSUME(!stack_is_empty(stack));

    return stack->bottom->data;
}

int
stack_is_empty(struct stack *stack)
{
    ASSUME(stack != NULL);

    return stack->bottom == NULL;
}
