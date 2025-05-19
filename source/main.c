#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "opts.h"
#include "renderer.h"
#include "colorscheme.h"
#include "ttf_quadruplet.h"
#include "error.h"
#include "io.inc"

char * font_name       = "dejavusansmono";
char * font_directory  = "/usr/share/fonts/";
char * output_filename = "xeen.png";

int font_size_opt = 24;
int tab_width     =  8;

/* Trusty Flex scanners.
 * NOTE: They are one use! They are freed after the first run!
 */
extern int get_dimensions(char * str, size_t n, int * h, int * w);
extern int xeen(char * str, size_t n);

signed main(const int argc, const char * const argv[]) {
  #define CHECKED_LOAD(x) do {     \
    font_style = font_ ## x;       \
    import_ttf_font(fonts. x);  \
  } while (0)
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

    renderer_init(w, h);

    CHECKED_LOAD(normal);
    CHECKED_LOAD(bold);
    CHECKED_LOAD(italic);
    CHECKED_LOAD(bold_italic);
    font_style = font_normal;
    xeen(input, input_len);

    if (export_png_image(output_filename)) {
        error(
            "Failed to export '%s' (this could be either an internal or filesystem error).",
            output_filename
        );
        return 1;
    }

    free(input);
    free_ttf_quadruplet(fonts);

    return 0;
  #undef CHECKED_LOAD
}
