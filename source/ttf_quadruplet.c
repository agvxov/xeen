#include "ttf_quadruplet.h"

#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <fts.h>
#include <alloca.h>

enum {
    TTF_NORMAL     = 0b00,
    TTF_BOLD       = 0b01,
    TTF_ITALIC     = 0b10,
    TTF_BOLDITALIC = 0b11,
};

static inline
int ttf_style(const char * name) {
    int r = TTF_NORMAL;

    if (strstr(name, "bold"))    { r |= TTF_BOLD; }
    if (strstr(name, "italic"))  { r |= TTF_ITALIC; }
    if (strstr(name, "oblique")) { r |= TTF_ITALIC; }

    return r;
}

ttf_quadruplet_t load_font(char * fonts_path, const char * target_name) {
    ttf_quadruplet_t r = (ttf_quadruplet_t){0};

    if (fonts_path == NULL) {
        fonts_path = strdup("/usr/share/fonts");
    }

    char * paths[] = {strdup(fonts_path), NULL};
    FTS *tree = fts_open(paths, FTS_PHYSICAL | FTS_NOCHDIR, NULL);
    if (!tree) { goto end; }

    FTSENT * entry;
    while ((entry = fts_read(tree)) != NULL) {
        if (entry->fts_info != FTS_F) { continue; }

        const char * path = entry->fts_path;
        const char * ext = strrchr(path, '.');
        if (!ext || strcmp(ext, ".ttf") != 0) { continue; }

        const char * base = entry->fts_name;

        char * lower_name = alloca(strlen(base) + 1);
        for (int i = 0; base[i] != '\0'; i++) {
            lower_name[i] = tolower(base[i]);
        }
        lower_name[strlen(base)] = '\0';

        if (!strstr(lower_name, target_name)) {
            continue;
        }

        int style = ttf_style(lower_name);
        const char * fullpath = entry->fts_path;
        switch (style) {
            case TTF_NORMAL:     r.normal     = strdup(fullpath); break;
            case TTF_BOLD:       r.bold       = strdup(fullpath); break;
            case TTF_ITALIC:     r.italic     = strdup(fullpath); break;
            case TTF_BOLDITALIC: r.bolditalic = strdup(fullpath); break;
        }

        if (is_quadruplet_full(r)) { break; }
    }

    fts_close(tree);

  end:
    return r;
}
