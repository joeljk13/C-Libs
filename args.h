#ifndef ARGS_H_
#define ARGS_H_ 1

enum arg_type {
    ARG_STRING,
    ARG_INTEGER,
    ARG_REAL,
    ARG_BOOL
};

void
args_init(int argc, char **argv);

void
args_free(void);

int
args_set_format(const char *format);

int
is_arg(const char *name);

void *
get_arg(const char *name);

#endif
