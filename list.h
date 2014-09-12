#ifndef LIST_H_
#define LIST_H_ 1

typedef int (*list_cmp_t)(void *, void *);

struct list {
    list_cmp_t cmp;
};

int
list_init(struct list *list, list_cmp_t cmp);

void
list_free(struct list *list);

int
list_add(struct list *list, void *data);

void
list_remove(struct list *list, void *data);

int
list_contains(struct list *list, void *data);

int
list_is_empty(struct list *list);

int
list_traverse(struct list *list, int (*func)(void *));

int
list_traverse_fast(struct list *list, int (*func)(void *));

#endif
