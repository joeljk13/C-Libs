#include "args.h"
#include "main.h"

#include "alloc.h"

#include <assert.h>
#include <stddef.h>
#include <string.h>

struct arg {
    const char *name;
    const char *value;
    int is_found;
    int is_registered;
};

static int argc_;
static char **argv_;

int
register_arg(const char *name, const char *abbrev, enum arg_type type)
{
    struct arg *arg_l, *arg_s;
    char *full_name, *full_abbrev;
    size_t name_len, name_prefix_len, abbrev_len, abbrev_prefix_len;

    if (IS_NULLPTR(name)) {
        return 1;
    }

    ASSUME(type == ARG_STRING || type == ARG_INTEGER || type == ARG_REAL ||
           type == ARG_BOOL);

    TODO("Check that strlen returns size_t");

    name_len = strlen(name);
    name_prefix_len = (*name != '-') * 2;
    abbrev_len = strlen(abbrev);
    abbrev_prefix_len = *abbrev != '-';

    full_name = MALLOC((name_len + name_prefix_len) * sizeof(*full_name));
    full_abbrev = (abbrev == NULL ? NULL :
                   MALLOC((abbrev_len + abbrev_prefix_len)
                          * sizeof(*full_abbrev)));
    arg_l = MALLOC(sizeof(*arg_l));
    arg_s = MALLOC(sizeof(*arg_s));
    if (IS_NULLPTR(full_name) || IS_NULLPTR(full_abbrev) || IS_NULLPTR(arg_l)
        || IS_NULLPTR(arg_s)) {
        FREE(full_name);
        FREE(full_abbrev);
        FREE(arg_l);
        FREE(arg_s);
        return 2;
    }

    TODO("Check what happens with memcpy for len = 0 and ptr = NULL");

    memcpy(full_name, "--", name_prefix_len);
    memcpy(full_name + name_prefix_len, name, name_len);
    memcpy(full_abbrev, "-", abbrev_prefix_len);
    memcpy(full_abbrev + abbrev_prefix_len, abbrev, abbrev_len);

    return 0;
}

void
set_args(int argc, char **argv)
{
    argc_ = argc;
    argv_ = argv;
}

int
is_arg(const char *arg)
{
    if (IS_NULLPTR(arg)) {
        return 0;
    }
}
