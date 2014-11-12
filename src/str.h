#ifndef VECTOR_H_
#define VECTOR_H_ 1

#include "main.h"

/* A vector of chars, A.K.A. a string. */
struct str {
    char *ptr;
    size_t length;
    size_t capacity;
};

/* All of the following functions take a struct str * as their first
 * argument. This pointer is always assumed not to be NULL.
 *
 * Any functions that take an index or multiple indices assume the the indices
 * are valid, and that begin <= end. */

/* Initializes the str. Returns 0 on success, nonzero on failure. */
int
str_init(struct str *str) NONNULL;

/* Sets all the chars to '\0'. */
void
str_zero(struct str *str) NONNULL;

/* Appends c to the end of str. Returns 0 on success, nonzero on failure. */
int
str_push(struct str *str, char c) NONNULL;

/* Appends each char in ptr to str. Returns 0 on success, nonzero on
 * failure. */
int
str_push_v(struct str *str, struct str *ptr) NONNULL;

/* Removes the last char from str and returns it. Assumes str is not
 * empty. */
char
str_pop(struct str *str) NONNULL;

/* Returns the last char in str. Assumes str is not empty. */
char
str_peek(struct str *str) NONNULL;

/* Inserts c into str at index, shifting all chars at and after index
 * by one. Returns 0 on success, nonzero on failure. */
int
str_insert(struct str *str, char c, size_t index) NONNULL;

/* Inserts each char in ptr to str starting at index, shifting all
 * chars after index in str by ptr->length. Returns 0 on succes, nonzero
 * on failure. */
int
str_insert_v(struct str *str, struct str *ptr, size_t index) NONNULL;

/* Removes the char at index from str, shifting each char after index
 * by one. */
void
str_remove(struct str *str, size_t index) NONNULL;

/* Removes the chars in [begin, end), and shifts the chars starting at
 * end by (end - begin). */
void
str_remove_r(struct str *str, size_t begin, size_t end) NONNULL;

/* Same as str_remove, but faster. However, this doesn't preserve the
 * ordering of the chars. */
void
str_remove_fast(struct str *str, size_t index) NONNULL;

/* Same as str_remove_r, but faster, at least for small ranges. However,
 * this doesn't preserve the ordering of the chars. */
void
str_remove_fast_r(struct str *str, size_t begin, size_t end) NONNULL;

/* Returns 1 if str contains c, otherwise returns 0. */
int
str_contains(struct str *str, char c) NONNULL;

/* Returns the index where c is. If c is not in str, returns
 * str->lengh. */
size_t
str_find(struct str *str, char c) NONNULL;

/* Resizes str to size. Returns 0 on success, nonzero on failure. */
int
str_resize(struct str *str, size_t size) NONNULL;

/* Reserves enough memory for at least size chars. Returns 0 on success, nonzero
 * on failure. */
int
str_reserve(struct str *str, size_t size) NONNULL;

/* Removes all chars outside the range [begin, end). Returns 0 on success,
 * nonzero on failure. */
void
str_slice(struct str *str, size_t begin, size_t end) NONNULL;

/* Frees the memory used by str. */
void
str_free(struct str *str) NONNULL;

#endif
