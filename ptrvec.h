#ifndef VECTOR_H_
#define VECTOR_H_ 1

struct ptrvec {
    void **ptr;
    size_t length;
    size_t capacity;
}

int
ptrvec_init(struct ptrvec *ptrvec);

void
ptrvec_zero(struct ptrvec *ptrvec);

int
ptrvec_push(struct ptrvec *ptrvec, void *ptr);

int
ptrvec_push_v(struct ptrvec *ptrvec, struct ptrvec *ptr);

// Also could be void
void *
ptrvec_pop(struct ptrvec *ptrvec);

void *
ptrvec_peek(struct ptrvec *ptrvec);

int
ptrvec_insert(struct ptrvec *ptrvec, void *ptr, size_t index);

int
ptrvec_insert_v(struct ptrvec *ptrvec, struct ptrvec *ptr, size_t index);

void
ptrvec_remove(struct ptrvec *ptrvec, size_t index);

void
ptrvec_remove_r(struct ptrvec *ptrvec, size_t begin, size_t end);

void
ptrvec_remove_fast(struct ptrvec *ptrvec, size_t index);

void
ptrvec_remove_fast_r(struct ptrvec *ptrvec, size_t begin, size_t end);

int
ptrvec_contains(struct ptrvec *ptrvec, void *ptr);

size_t
ptrvec_find(struct ptrvec *ptrvec, void *ptr);

int
ptrvec_resize(struct ptrvec *ptrvec, size_t size);

int
ptrvec_reserve(struct ptrvec *ptrvec, size_t size);

int
ptrvec_slice(struct ptrvec *ptrvec, size_t begin, size_t end);

// Or maybe struct ptrvec **
struct ptrvec *
ptrvec_split(struct ptrvec *ptrvec, void *ptr);

void
ptrvec_free(struct ptrvec *ptrvec);

#endif
