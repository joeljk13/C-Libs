#ifndef ARGS_H_
#define ARGS_H_ 1

void
init_args(int argc, char **argv);

int
is_arg(const char *arg);

const char *
get_arg(const char *arg);

void
free_args(void);

#endif
