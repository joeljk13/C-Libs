#ifndef STACK_H_
#define STACK_H_ 1

struct stack_elem;

struct stack {
    struct stack_elem *bottom;
};

void
stack_init(struct stack *stack);

void
stack_free(struct stack *stack);

int
stack_push(struct stack *stack, void *data);

void *
stack_pop(struct stack *stack);

void *
stack_peek(struct stack *stack);

#endif
