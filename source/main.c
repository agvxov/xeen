#include <stdio.h>
#include "sds.h"
#include "opts.h"
#include "renderer.h"
#include "colorscheme.h"
#include "ttf_quadruplet.h"

extern int get_dimensions(char * str, size_t n, int * h, int * w);
extern int xeen(char * str, size_t n);

char * font_filename   = "dejavusansmono";
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

    // This will be fixed once we have font_dir and font_name thing.
    // It should be handled in CLI arguments I think...
    ttf_quadruplet_t font_name = load_font(NULL, font_filename);

    if (!is_quadruplet_full(font_name)) {
        // Please provide normal error message?
        // Also, we don't need this? It's used in source as loop break.
        // Will load_font enter infinite loop on incorrect font name?
        fprintf(stderr, "Oh fuck oh hell...\n");
        return 1;
    }

    font_style = font_normal;      import_ttf_font(font_name.normal);
    font_style = font_bold;        import_ttf_font(font_name.bold);
    font_style = font_italic;      import_ttf_font(font_name.italic);
    font_style = font_bold_italic; import_ttf_font(font_name.bolditalic);
    font_style = font_normal;

    render_defaults(w, h);
    xeen(input, input_len);

    if (export_png_image(output_filename)) {
        return 1;
    }

    sdsfree(input);

    return 0;
}
