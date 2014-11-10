#ifndef ARGS_H_
#define ARGS_H_ 1

#include "main.h"

enum arg_type {
    ARG_BOOL,

    ARG_STRING,

    ARG_CHAR,
    ARG_SIGNED_CHAR,
    ARG_UNSIGNED_CHAR,

    ARG_SHORT,
    ARG_SIGNED_SHORT,
    ARG_UNSIGNED_SHORT,

    ARG_INT,
    ARG_SIGNED_INT,
    ARG_UNSIGNED_INT,

    ARG_LONG,
    ARG_SIGNED_LONG,
    ARG_UNSIGNED_LONG,

    ARG_LONG_LONG,
    ARG_SIGNED_LONG_LONG,
    ARG_UNSIGNED_LONG_LONG,

    ARG_FLOAT,
    ARG_DOUBLE,
    ARG_LONG_DOUBLE,

    ARG_SIZE_T
};

/* Initializes the args. Returns 0 on success, nonzero on failure. */
int
args_init(size_t argc, char **argv, const char *format) NONNULL_AT(2);

/* Cleans up resources used by the args. */
void
args_free(void);

/* Returns 1 if name is an argument that was passed, 0 otherwise. */
int
is_arg(const char *name) NONNULL;

/* Returns NULL is name is not an argument that was passed, otherwise a pointer
 * to the value of the argument. */
void *
get_arg(const char *name) NONNULL;

#endif
