#ifndef ERROR_H
#define ERROR_H

#include <stdio.h>
#include <stdarg.h>

static inline
void warning(const char * fmt, ...) {
    va_list args;
    va_start(args, fmt);

    fprintf(stderr, "Warning: ");
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");

    va_end(args);
}

static inline
void error(const char * fmt, ...) {
    va_list args;
    va_start(args, fmt);

    fprintf(stderr, "Error: ");
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");

    va_end(args);
}

#endif
