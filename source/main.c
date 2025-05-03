#include <stdio.h>
#include "sds.h"
#include "opts.h"
#include "renderer.h"
#include "colorscheme.h"
#include "ttf_quadruplet.h"

extern int get_dimensions(char * str, size_t n, int * h, int * w);
extern int xeen(char * str, size_t n);

char * font_name       = "dejavusansmono";
char * font_directory  = "/usr/share/fonts/TTF/";
char * output_filename = "xeen.png";

int font_size_opt = 24;
int tab_width     =  8;

static
sds stdin2str(void) {
    const int READ_BATCH_SIZE = 64;
    sds r = sdsnew("");

    char buffer[READ_BATCH_SIZE + 1];
    int read_count = 0;
    do {
        read_count = fread(buffer, 1, READ_BATCH_SIZE, stdin);
        r = sdscatlen(r, buffer, read_count);
    } while (read_count == READ_BATCH_SIZE);

    return r;
}

signed main(const int argc, const char * const argv[]) {
    parse_args(argc, argv);

    sds input        = stdin2str();
    size_t input_len = sdslen(input);

    int w, h;
    get_dimensions((char*)input, input_len, &h, &w);
    render_fg = default_color;
    render_bg = default_background;
    font_size = font_size_opt;

    ttf_quadruplet_t fonts = load_font(font_directory, font_name);

    if (!is_quadruplet_full(fonts)) {
        fprintf(stderr, "Failed to load font '%s' from '%s'.\n", font_name, font_directory);
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
        return 1;
    }

    sdsfree(input);

    return 0;
}
