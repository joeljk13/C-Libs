#include "stack.h"
#include "main.h"

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
    struct stack_elem *to_free, *prev;

    ASSUME(stack != NULL);

    if (stack->bottom == NULL) {
        return;
    }

    to_free = stack->bottom;
    prev = to_free->prev;
    free(to_free);

    while (prev != NULL) {
        to_free = prev;
        prev = to_free->prev;
        free(to_free);
    }
}

int
stack_push(struct stack *stack, void *data)
{
    struct stack_elem *elem;

    ASSUME(stack != NULL);

    elem = malloc(sizeof(*elem));
    if (ERR_IS_NULLPTR(elem)) {
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

    if (stack->bottom == NULL) {
        return NULL;
    }

    elem = stack->bottom;
    stack->bottom = elem->prev;

    data = elem->data;

    free(elem);

    return data;
}

void *
stack_peek(struct stack *stack)
{
    ASSUME(stack != NULL);

    return stack->bottom == NULL ? NULL : stack->bottom->data;
}
