#include <stdio.h>
#include "sds.h"
#include "opts.h"
#include "renderer.h"
#include "colorscheme.h"

extern int get_dimensions(char * str, size_t n, int * h, int * w);
extern int xeen(char * str, size_t n);

char * font_filename   = "resource/terminus";
char * output_filename = "xeen.png";

int font_size_opt = 24;
int tab_width     =  8;

#define READ_BATCH_SIZE 64

sds stdin2str(void) {
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

    for (font_type i = 0; i < font_types; ++i) {
        const char * font_name[font_types] = {
            "normal",
            "bold",
            "italic",
            "bold_italic",
        };
        char buf[0x666] = "";
        snprintf(buf, 0x666, "%s/%s.ttf", font_filename, font_name[i]);
        font_style = i;
        if (import_ttf_font(buf)) {
            return 1;
        }
    }

    font_style = font_normal;

    render_defaults(w, h);
    xeen(input, input_len);

    if (export_png_image(output_filename)) {
        return 1;
    }

    sdsfree(input);

    return 0;
}
