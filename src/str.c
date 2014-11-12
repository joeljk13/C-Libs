#include "main.h"
#include "str.h"

#include "alloc.h"
#include "vec.h"

#include <string.h>

int
str_init(struct str *str)
{
    ASSUME(str != NULL);

    str->ptr = NULL;
    str->length = 0;
    str->capacity = 0;

    return 0;
}

void
str_zero(struct str *str)
{
    ASSUME(str != NULL);

    memset(str->ptr, 0, str->length);
}

int
str_push(struct str *str, char c)
{
    ASSUME(str != NULL);

    if (str->capacity == str->length
        && ERR(vec_reserve_one_min(&str->ptr, &str->capacity,
                                   sizeof(*str->ptr)) != 0)) {

        return -1;
    }

    str->ptr[str->length++] = c;

    return 0;
}

int
str_push_v(struct str *str, struct str *ptr)
{
    ASSUME(str != NULL);
    ASSUME(ptr != NULL);

    if (ERR(str_reserve(str, str->length + ptr->length) != 0)) {
        return -1;
    }

    memcpy(str->ptr + str->length, ptr->ptr, ptr->length *
           sizeof(*ptr->ptr));

    str->length += ptr->length;

    return 0;
}

char
str_pop(struct str *str)
{
    ASSUME(str != NULL);

    return str->ptr[--str->length];
}

char
str_peek(struct str *str)
{
    ASSUME(str != NULL);

    return str->ptr[str->length - 1];
}

int
str_insert(struct str *str, char c, size_t index)
{
    ASSUME(str != NULL);
    ASSUME(index <= str->length);

    if (str->length == str->capacity
        && ERR(vec_reserve_one_min(&str->ptr, &str->capacity,
                                   sizeof(*str->ptr)) != 0)) {

        return -1;
    }

    memmove(str->ptr + index + 1, str->ptr + index,
            (str->length - index - 1) * sizeof(*str->ptr));

    str->ptr[index] = c;

    ++str->length;

    return 0;
}

int
str_insert_v(struct str *str, struct str *ptr, size_t index)
{
    ASSUME(str != NULL);
    ASSUME(ptr != NULL);
    ASSUME(index <= str->length);

    if (ERR(str_reserve(str, str->length + ptr->length) != 0)) {
        return -1;
    }

    memmove(str->ptr + index + ptr->length, str->ptr + index,
            ptr->length * sizeof(*ptr->ptr));
    // If it's inserting a str inside itself, it'll need a memmove
    memmove(str->ptr + index, ptr->ptr, ptr->length * sizeof(*ptr->ptr));

    str->length += ptr->length;

    return 0;
}

void
str_remove(struct str *str, size_t index)
{
    ASSUME(str != NULL);
    ASSUME(index < str->length);

    memmove(str->ptr + index, str->ptr + index + 1,
            (str->length - index - 1) * sizeof(*str->ptr));

    --str->length;
}

void
str_remove_r(struct str *str, size_t begin, size_t end)
{
    ASSUME(str != NULL);
    ASSUME(begin <= end);
    ASSUME(end <= str->length);

    memmove(str->ptr + begin, str->ptr + end, (str->length - end)
            * sizeof(*str->ptr));

    str->length -= (end - begin);
}

void
str_remove_fast(struct str *str, size_t index)
{
    ASSUME(str != NULL);
    ASSUME(index <= str->length);

    str->ptr[index] = str->ptr[--str->length];
}

void
str_remove_fast_r(struct str *str, size_t begin, size_t end)
{
    ASSUME(str != NULL);
    ASSUME(begin <= end);
    ASSUME(end <= str->length);

    memmove(str->ptr + begin, str->ptr + str->length - (end - begin),
            (end - begin) * sizeof(*str->ptr));

    str->length -= (end - begin);
}

int
str_contains(struct str *str, char c)
{
    ASSUME(str != NULL);

    return strchr(str->ptr, c) != NULL;
}

size_t
str_find(struct str *str, char c)
{
    const char *tmp;

    ASSUME(str != NULL);

    tmp = strchr(str->ptr, c);

    return tmp == NULL ? str->length : tmp - str->ptr;
}

int
str_resize(struct str *str, size_t size)
{
    void *tmp;

    ASSUME(str != NULL);

    if (size <= str->capacity) {
        for (size_t i = str->length; i < size; ++i) {
            str->ptr[i] = '\0';
        }

        str->length = size;

        return 0;
    }

    if (ERR(str_reserve(str, size) != 0)) {
        return -1;
    }

    for (size_t i = str->length; i < size; ++i) {
        str->ptr[i] = '\0';
    }

    str->length = str->capacity = size;

    return 0;
}

int
str_reserve(struct str *str, size_t size)
{
    void *tmp;

    ASSUME(str != NULL);

    if (size <= str->capacity) {
        return 0;
    }

    if (ERR(vec_reserve_min(&str->ptr, &str->capacity,
                            sizeof(*str->ptr), size - str->length)
            != 0)) {

        return -1;
    }

    return 0;
}

void
str_slice(struct str *str, size_t begin, size_t end)
{
    ASSUME(str != NULL);
    ASSUME(begin <= end);
    ASSUME(end <= str->length);

    str_remove_r(str, end, str->length);
    str_remove_r(str, 0, begin);
}

void
str_free(struct str *str)
{
    ASSUME(str != NULL);

    jfree(str->ptr);
}
