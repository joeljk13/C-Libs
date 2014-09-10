#ifndef ARGS_H_
#define ARGS_H_ 1

enum arg_type {
    ARG_STRING,
    ARG_INTEGER,
    ARG_REAL,
    ARG_BOOL
};

/* Initializes the args. Returns 0 on success, -1 on failure. */
int
args_init(int argc, char **argv, const char *format);

/* Cleans up resources used by the args. */
void
args_free(void);

/* Returns 1 if name is an argument that was passed, 0 otherwise. */
int
is_arg(const char *name);

/* Returns NULL is name is not an argument that was passed, otherwise a pointer
 * to the value of the argument. */
void *
get_arg(const char *name);

#endif
