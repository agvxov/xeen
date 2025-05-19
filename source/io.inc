#include <string.h>
#include "error.h"

static inline
char * stdin2str(size_t * len) {
    const int READ_BATCH_SIZE = 1024;
    char * r = NULL;
    size_t r_len = 0;

    char buffer[READ_BATCH_SIZE + 1];
    int read_count = 0;
    do {
        read_count = fread(buffer, 1, READ_BATCH_SIZE, stdin);
        r = realloc(r, r_len + read_count);
        memcpy(r + r_len, buffer, read_count);
        r_len += read_count;
    } while (read_count == READ_BATCH_SIZE);
    r = realloc(r, r_len + 1);
    r[r_len] = '\0';

    *len = r_len;

    return r;
}

static inline
char * read_entire_file(const char * name) {
    char * r;

    FILE * font_file = fopen(name, "rb");

    if (!font_file) {
        error("Failed to open font file '%s'.", name);
        return NULL;
    }

    fseek(font_file, 0, SEEK_END);
    long font_size_bytes = ftell(font_file);
    fseek(font_file, 0, SEEK_SET);

    r = malloc(font_size_bytes);
    fread(r, 1, font_size_bytes, font_file);
    fclose(font_file);

    return r;
}
