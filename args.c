#include "main.h"
#include "args.h"

#include "alloc.h"
#include "ptrvec.h"

#include <assert.h>
#include <stddef.h>
#include <string.h>

enum arg_props {
    ARG_NULL        = 0x0,
    ARG_IS_FOUND    = 0x1,
    ARG_MULTI_CASE  = 0x2,
    ARG_PREFIXED    = 0x4
};

struct arg {
    const char *name;
    const char *abbrev;
    void *value;
    enum arg_type type;
    enum arg_props props;
};

enum {
    FORMAT_OPTIONS_BEGIN    = '<',
    FORMAT_OPTIONS_END      = '>',
    FORMAT_OPTIONAL_BEGIN   = '[',
    FORMAT_OPTIONAL_END     = ']',
    FORMAT_ORDERED_BEGIN    = '{',
    FORMAT_ORDERED_END      = '}',
    FORMAT_UNORDERED_BEGIN  = '(',
    FORMAT_UNORDERED_EN     = ')',

    FORMAT_DEFAULT_BEGIN    = FORMAT_UNORDERED_BEGIN,
    FORMAT_DEFAULT_END      = FORMAT_UNORDERED_END,

    FORMAT_OPTIONS_NOPREPEND        = 'p',
    FORMAT_OPTIONS_PREPEND          = 'P',
    FORMAT_OPTIONS_CASE_INSENSITIVE = 'c',
    FORMAT_OPTIONS_CASE_SENSITIVE   = 'C'
};

/* Initialize to {0} to set all options to the defaults */
struct format_options {
    int case_insensitive;
    int noprepend;
};

struct format_data {
    union {
        struct arg arg;
        char data;
    };

    enum {
        FORMAT_GROUPING,
        FORMAT_META_DATA,
        FORMAT_ARG
    } type;
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

/* Sets *index to after the closing '>'. */
static int
parse_options(const char *format, struct format_options *options,
              size_t *index)
{
    ASSUME(format != NULL);
    ASSUME(options != NULL);
    ASSUME(index != NULL);

    ASSERT(format[*index] == FORMAT_OPTIONS_BEGIN);

    // Skip the '<'
    for (++*index; ; ++*index) {
        switch (format[i]) {

        case FORMAT_OPTIONS_PREPEND:
            options->noprepend = 0;
            break;

        case FORMAT_OPTIONS_NOPREPEND:
            options->noprepend = 1;
            break;

        case FORMAT_OPTIONS_CASE_INSENSITIVE:
            options->case_insensitive = 0;
            break;

        case FORMAT_OPTIONS_CASE_SENSITIVE:
            options->case_insensitive = 1;
            break;

        case FORMAT_OPTIONS_END:
            // Make sure format + index points to the character after
            // FORMAT_OPTIONS_END
            ++*index;

        case '\0':
            return 0;

        default:
            return -1;
        }
    }

    ASSUME_UNREACHABLE();
}

int
lex_format(const char *format, struct ptrvec *stack)
{
    ASSUME(format != NULL);
    ASSUME(stack != NULL);

    return 0;
}

static int
parse_format(const char *format)
{
    int err;
    struct ptrvec stack;
    struct format_options options = {0};
    struct format_data first, last;

    ASSUME(format != NULL);

    if (ERR(ptrvec_init(&stack) != 0)) {
        return -1;
    }

    err = 0;

    first = (struct format_data){
        .type: FORMAT_GROUPING,
        .data: '('
    };

    if (ERR(ptrvec_push(&stack, &def_group_char) != 0)) {
        err = -1;
        goto exit;
    }

    last = (struct format_data){
        .type: FORMAT_GROUPING,
        .data: ')'
    };

    for (size_t i = 0; ; ++i) {
        switch (format[i]) {

        case FORMAT_META_BEGIN:
            if (ERR(parse_meta_data(format, &meta, &i) != 0)) {
                err = -1;
                goto exit;
            }

            break;

        case FORMAT_ORDERED_BEGIN:
        case FORMAT_UNORDERED_BEGIN:
        case FORMAT_OPTIONAL_BEGIN:
            if (ERR(ptrvec_push(&stack, format + i) != 0)) {
                err = -1;
                goto exit;
            }

            break;

        case FORMAT_ORDERED_END:
            if (ERR(stack.length == 0) || ERR(*ptrvec_pop(&stack) !=
                                              FORMAT_ORDERED_BEGIN)) {

                err = -1;
                goto exit;
            }

            break;

        case FORMAT_UNORDERED_END:
            if (ERR(stack.length == 0) || ERR(*ptrvec_pop(&stack) !=
                                              FORMAT_UNOREDERED_BEGIN)) {

                err = -1;
                goto exit;
            }

            break;

        case FORMAT_OPTIONAL_END:
            if (ERR(stack.length == 0) || ERR(*ptrvec_pop(&stack) !=
                                              FORMAT_OPTIONAL_BEGIN)) {

                err = -1;
                goto exit;
            }

            break;
        }
    }

exit:

    ptrvec_free(&stack);

    return err;
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
