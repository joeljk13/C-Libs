#include "main.h"
#include "file.h"

#include <stdint.h>
#include <string.h>

size_t
flen(FILE *file)
{
    size_t len;
    fpos_t curpos;

    ASSUME(file != NULL);

    if (ERR(fgetpos(file, &curpos) != 0)) {
        return SIZE_MAX;
    }

    rewind(file);

    len = 0;

    for (int c; (c = fgetc(file)) != EOF; ++len) { }

    if (ERR(fsetpos(file, &curpos) != 0)) {
        return SIZE_MAX;
    }

    return len;
}

fpos_t *
fchr(FILE *file, int character)
{
    fpos_t pos, curpos;

    ASSUME(file != NULL);

    if (ERR(fgetpos(file, &curpos) != 0)) {
        return NULL;
    }

    for (int c; ; ) {
        c = fgetc(file);

        if (c == EOF) {
            fsetpos(file, &curpos);
            return NULL;
        }

        if ((char)c == (char)character) {
            if (ERR(fgetpos(file, &pos) != 0)) {
                fsetpos(file, &curops);
                return NULL;
            }

            fsetpos(file, &curpos);

            return &pos;
        }
    }

    ASSUME_UNREACHABLE();
}

size_t
fcspn(FILE *file, const char *str)
{
    fpos_t curpos;
    size_t len;

    ASSUME(file != NULL);
    ASSUME(str != NULL);

    if (ERR(fgetpos(file, &curpos) != 0)) {
        return SIZE_MAX;
    }

    len = 0;

    for (int c; (c = fgetc(file)) != EOF && strchr(str, c) == NULL; ++len) { }

    if (ERR(fsetpos(file, &curpos) != 0)) {
        return SIZE_MAX;
    }

    return len;
}

// TODO - make this return a const char *
fpos_t *
fpbrk(FILE *file, const char *str)
{
    fpos_t pos, curpos;

    ASSUME(file != NULL);
    ASSUME(str != NULL);

    if (ERR(fgetpos(file, &curpos) != 0)) {
        return NULL;
    }

    for (int c; ; ) {
        c = fgetc(file);

        if (c == EOF) {
            fsetpos(file, &curpos);
            return NULL;
        }

        if (strchr(str, c) != NULL) {
            if (ERR(fgetpos(file, &pos) != 0)) {
                fsetpos(file, &curops);
                return NULL;
            }

            fsetpos(file, &curpos);

            return &pos;
        }
    }

    ASSUME_UNREACHABLE();
}

fpos_t *
frchr(FILE *file, int character)
{
    ASSUME(file != NULL);

    TODO(implement frchr);

    return NULL;
}

size_t
fspn(FILE *file, const char *str)
{
    fpos_t curpos;
    size_t len;

    ASSUME(file != NULL);
    ASSUME(str != NULL);

    if (ERR(fgetpos(file, &curpos) != 0)) {
        return SIZE_MAX;
    }

    len = 0;

    for (int c; (c = fgetc(file)) != EOF && strchr(str, c) != NULL; ++len) { }

    if (ERR(fsetpos(file, &curpos) != 0)) {
        return SIZE_MAX;
    }

    return len;
}

fpos_t *
fstr(FILE *file, const char *str)
{
    fpos_t pos, curpos;

    ASSUME(file != NULL);
    ASSUME(str != NULL);

    if (ERR(fgetpos(file, &curpos) != 0)) {
        return NULL;
    }

    for (int c, i = 0; ; ) {
        if (str[i] == '\0') {
            if (ERR(fgetpos(file, &pos) != 0)) {
                fsetpos(file, &curpos);
                return NULL;
            }

            return &pos;
        }

        c = fgetc(file);

        if (c == EOF) {
            break;
        }

        if (str[i] != (char)c) {
            i = 0;
        }
    }

    fsetpos(file, &curpos);

    return NULL;
}

const char *
ftok(FILE *file, const char *delims)
{
    char *str;

    ASSUME(file != NULL);
    ASSUME(delims != NULL);

    for (int c; (c = fgetc(file)) != EOF; ) {

    }
}
