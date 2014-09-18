#include "args.h"
#include "main.h"

#include "alloc.h"

#include <assert.h>
#include <stddef.h>
#include <string.h>

enum arg_props {
    ARG_IS_FOUND = 1,
    ARG_MULTI_CASE = 2,
    ARG_PREFIXED = 4
};

struct arg {
    const char *name;
    const char *abbrev;
    void *value;
    enum arg_type type;
    enum arg_props props;
};

static int argc_ = 0;
static const char **argv_ = NULL;

static int n_args = 0;
static struct arg *args = NULL;

static inline int
register_arg(const char *name, const char *abbrev, enum arg_type type)
{
    char *full_name, *full_abbrev;
    size_t name_len, name_prefix_len, abbrev_len, abbrev_prefix_len;

    ASSUME(name != NULL);
    ASSUME(type == ARG_STRING || type == ARG_INTEGER || type == ARG_REAL ||
           type == ARG_BOOL);

    name_len = strlen(name);
    name_prefix_len = (*name != '-') * 2;
    if (abbrev != NULL) {
        abbrev_len = strlen(abbrev);
        abbrev_prefix_len = *abbrev != '-';
    }

    full_name = MALLOC((name_len + name_prefix_len) * sizeof(*full_name));
    full_abbrev = (abbrev == NULL ? NULL :
                   MALLOC((abbrev_len + abbrev_prefix_len)
                          * sizeof(*full_abbrev)));
    if (ERR_IS_NULLPTR(full_name) || ERR_IS_NULLPTR(full_abbrev)) {
        FREE(full_name);
        FREE(full_abbrev);

        return -1;
    }

    memcpy(full_name, "--", name_prefix_len);
    memcpy(full_name + name_prefix_len, name, name_len);
    memcpy(full_abbrev, "-", abbrev_prefix_len);
    memcpy(full_abbrev + abbrev_prefix_len, abbrev, abbrev_len);

    args[n_args++] = (struct arg){full_name, full_abbrev, NULL, type, 0};

    return 0;
}

int
parse_format(const char *format)
{
    ASSUME(format != NULL);
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
    // shouldn't matter.
    args = MALLOC(argc_ * sizeof(*args));
    if (ERR_IS_NULLPTR(args)) {
        return -1;
    }

    if (format == NULL) {
        // There was no format specified, so use the raw args. Basically 
        // register each arg that exists and say that it was found.
        for (int i = 0; i < argc_; ++i) {
            args[i] = (struct arg){argv[i], NULL, NULL, ARG_BOOL, 1};
        }

        return 0;
    }
    else {
        // This will probably zero args, but just in case do this explicity
        // instead of using CALLOC or MALLOC + memset
        for (int i = 0; i < argc_; ++i) {
            args = (struct arg){NULL, NULL, NULL, ARG_BOOL, 0};
        }

        if (ERR(parse_format(format) != 0)) {
            return -1;
        }
    }

    return 0;
}

void
args_free(void)
{
    for (int i = 0; i < n_args; ++i) {
        FREE(args[i].name);
        FREE(args[i].abbrev);
        FREE(args[i].value);
    }

    FREE(args);
}

int
is_arg(const char *name)
{
    size_t name_prefix_len;

    ASSUME(name != NULL);

    name_prefix_len = (*name != '-') * 2;

    TODO(Implement is_arg);
}
