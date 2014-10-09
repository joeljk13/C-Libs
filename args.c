#include "main.h"
#include "args.h"

#include "alloc.h"
#include "ptrvec.h"

#include <assert.h>
#include <stddef.h>
#include <string.h>

#define FORMAT_META_BEGIN '<'
#define FORMAT_META_END '>'
#define FORMAT_OPTIONAL_BEGIN '['
#define FORMAT_OPTIONAL_END ']'
#define FORMAT_ORDERED_BEGIN '{'
#define FORMAT_ORDERED_END '}'
#define FORMAT_UNORDERED_BEGIN '('
#define FORMAT_UNORDERED_BEGIN ')'

#define FORMAT_DEFAULT_BEGIN FORMAT_UNORDERED_BEGIN
#define FORMAT_DEFAULT_END FORMAT_UNORDERED_END

#define FORMAT_META_NOPREPEND 'p'
#define FORMAT_META_PREPEND 'P'
#define FORMAT_META_CASE_INSENSITIVE 'c'
#define FORMAT_META_CASE_SENSITIVE 'C'

enum arg_props {
    ARG_NULL = 0,
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
    ASSUME(type == ARG_STRING || type == ARG_INTEGER || type == ARG_REAL
           || type == ARG_BOOL);

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

    args[n_args++] = (struct arg){full_name, full_abbrev, NULL, type,
        ARG_NULL};

    return 0;
}

struct format_meta {
    int case_insensitive;
    int noprepend;
};

/* Sets *index to after the closing '>'. */
static int
parse_meta_data(const char *format, struct format_meta *data, size_t *index)
{
    ASSUME(format != NULL);
    ASSUME(data != NULL);
    ASSUME(index != NULL);

    ASSERT(*index == '<');

    // Skip the '<'
    for (size_t i = ++*index; ; ++i) {
        // *index should equal i + 1, so that when format[i] ==
        // FORMAT_META_END, format + index points to the first non-meta char
        ++*index;
        switch (format[i]) {
        case FORMAT_META_PREPEND:
            data->noprepend = 0;
            break;

        case FORMAT_META_NOPREPEND:
            data->noprepend = 1;
            break;

        case FORMAT_META_CASE_INSENSITIVE:
            data->case_insensitive = 0;
            break;

        case FORMAT_META_CASE_SENSITIVE:
            data->case_insensitive = 1;
            break;

        case FORMAT_META_END:
            return 0;

        default:
            return -1;

        }
    }

    ASSUME_UNREACHABLE();
}

static int
parse_format(const char *format)
{
    struct ptrvec stack;
    struct format_meta = {0};

    const char def_group_char = FORMAT_DEFAULT_BEGIN;

    ASSUME(format != NULL);

    if (ERR(ptrvec_init(&stack) != 0)) {
        return -1;
    }

    if (ERR(ptrvec_push(&stack, &def_group_char) != 0)) {
        ptrvec_free(&stack);
        return -1;
    }

    for (size_t i = 0; ; ++i) {
        switch (format[i]) {
        case '<':
            if (ERR(parse_meta_data(format, &format_meta, &i) != 0)) {
                ptrvec_free(&stack);
                return -1;
            }
            break;

        case '{':
        case '(':
        case '[':
            if (ERR(ptrvec_push(&stack, format + i) != 0)) {
                ptrvec_free(&stack);
                return -1;
            }
            break;
        }
    }

    ptrvec_free(&stack);

    return 0;
}

int
args_init(int argc, const char **argv, const char *format)
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
    if (ERR(args == NULL)) {
        return -1;
    }

    if (format == NULL) {
        // There was no format specified, so use the raw args. Basically 
        // register each arg that exists and say that it was found.
        for (int i = 0; i < argc_; ++i) {
            args[i] = (struct arg){argv[i], NULL, NULL, ARG_BOOL, ARG_IS_FOUND};
        }

        return 0;
    } else {
        for (int i = 0; i < argc_; ++i) {
            args = (struct arg){0};
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
