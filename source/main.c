#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "opts.h"
#include "renderer.h"
#include "colorscheme.h"
#include "ttf_quadruplet.h"
#include "error.h"

//~XOLATILE SPECIFIC FONT!
//~char * font_name       = "terminus";
//~ANON SPECIFIC FONT?
char * font_name       = "dejavusansmono";
char * font_directory  = "/usr/share/fonts/";
char * output_filename = "xeen.png";

int font_size_opt = 24;
int tab_width     =  8;

extern int get_dimensions(char * str, size_t n, int * h, int * w);
extern int xeen(char * str, size_t n);

static
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

signed main(const int argc, const char * const argv[]) {
    parse_args(argc, argv);

    size_t input_len;
    char * input = stdin2str(&input_len);

    int w, h;
    get_dimensions((char*)input, input_len, &h, &w);
    render_fg = default_color;
    render_bg = default_background;
    font_size = font_size_opt;

    ttf_quadruplet_t fonts = load_font_paths(font_directory, font_name);

    if (!is_quadruplet_full(fonts)) {
        error("Failed to load font '%s' from '%s'.", font_name, font_directory);
        return 1;
    }

    font_style = font_normal;      import_ttf_font(fonts.normal);
    font_style = font_bold;        import_ttf_font(fonts.bold);
    font_style = font_italic;      import_ttf_font(fonts.italic);
    font_style = font_bold_italic; import_ttf_font(fonts.bolditalic);
    font_style = font_normal;

    render_defaults(w, h);
    xeen(input, input_len);

    if (export_png_image(output_filename)) {
        error(
            "Failed to export '%s' (this could be either an internal or filesystem error).",
            output_filename
        );
        return 1;
    }

    free(input);

    return 0;
}
