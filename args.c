#include "args.h"
#include "main.h"

#include "alloc.h"

#include <assert.h>
#include <stddef.h>
#include <string.h>

struct arg {
    const char *name;
    const char *value;
    enum arg_type type;
};

static int argc_ = 0;
static const char **argv_ = NULL;

static int n_args = 0;
static struct arg *args = NULL;

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

        return -1;
    }

    memcpy(full_name, "--", name_prefix_len);
    memcpy(full_name + name_prefix_len, name, name_len);
    memcpy(full_abbrev, "-", abbrev_prefix_len);
    memcpy(full_abbrev + abbrev_prefix_len, abbrev, abbrev_len);

    for (int i = 1; i < argc_; ++i) {
        if (strcmp(argv_[i], full_name) || strcmp(argv_[i], full_abbrev)) {
            args[n_args++] = (struct arg){full_name, full_abbrev, type};
        }
    }

    return 0;
}

int
args_init(unsigned int argc, const char **argv, const char *format)
{
    ASSUME(argc >= 1);
    ASSUME(argv != NULL);

    // Ignore the name of the process itself
    argc_ = argc - 1;
    argv_ = argv + 1;

    // Some slots may be left empty, but no more than argc_ slots will ever be
    // used. argc_ is probably small enough that a little extra memory
    // shouldn't matter
    args = MALLOC(argc_ * sizeof(*args));
    if (ERR_IS_NULLPTR(args)) {
        return -1;
    }

    return 0;
}

void
args_free(void)
{
    FREE(args);
}

int
is_arg(const char *arg)
{
    ASSUME(arg != NULL);
}
