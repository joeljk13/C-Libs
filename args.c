#include "args.h"

#include <assert.h>
#include <stddef.h>

struct arg {
    const char *name;
    const char *value;
};

static int argc_;
static struct arg *argv_;

void
set_args(int argc, char **argv)
{
    argc_ = argc;
    argv_ = argv;
}

int
is_arg(const char *arg)
{
    assert(arg != NULL);
}
