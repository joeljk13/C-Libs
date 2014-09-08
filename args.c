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

static int argc_ = 0;
static char **argv_ = NULL;

static int n_args = 0;
static struct arg *args = NULL;

void
args_init(int argc, char **argv)
{
    argc_ = argc;
    argv_ = argv;

    args = MALLOC(argc_ * sizeof(*args));
}

void
args_free(void)
{
    for (int i = n_args - 1; i >= 0; --i) {
        FREE(args[i]);
    }
    FREE(args);
}

static int
register_arg(const char *name, const char *abbrev, enum arg_type type)
{
    struct arg *arg_l, *arg_s;
    char *full_name, *full_abbrev;
    size_t name_len, name_prefix_len, abbrev_len, abbrev_prefix_len;

    ASSUME(type == ARG_STRING || type == ARG_INTEGER || type == ARG_REAL ||
           type == ARG_BOOL);

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
    if (ERR_IS_NULLPTR(full_name) || ERR_IS_NULLPTR(full_abbrev) || ERR_IS_NULLPTR(arg_l)
        || ERR_IS_NULLPTR(arg_s)) {
        FREE(full_name);
        FREE(full_abbrev);
        FREE(arg_l);
        FREE(arg_s);

        return 3;
    }

    memcpy(full_name, "--", name_prefix_len);
    memcpy(full_name + name_prefix_len, name, name_len);
    memcpy(full_abbrev, "-", abbrev_prefix_len);
    memcpy(full_abbrev + abbrev_prefix_len, abbrev, abbrev_len);

    for (int i = argc_ - 1; i > 0; --i) {
        if (strcmp(argv_[i], full_name) || strcmp(argv_[i], full_abbrev)) {
            struct arg *arg;

            arg = MALLOC(sizeof(*arg));
        }
    }

    return 0;
}

int
args_set_format(const char *format)
{
    if (ERR_IS_NULLPTR(format)) {
        return 1;
    }
}

int
is_arg(const char *arg)
{
    if (ERR_IS_NULLPTR(arg)) {
        return 0;
    }
}
