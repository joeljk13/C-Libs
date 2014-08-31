#ifndef ARGS_H_
#define ARGS_H_ 1

enum arg_type {
    ARG_STRING,
    ARG_INTEGER,
    ARG_REAL,
    ARG_BOOL
};

int
register_arg(const char *name, const char *abbrev, enum arg_type type);

void
args_init(int argc, char **argv);

int
is_arg(const char *name);

void *
get_arg(const char *name);

void
args_free(void);

#endif
