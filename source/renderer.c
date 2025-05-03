#include "renderer.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define STB_TRUETYPE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb_truetype.h"
#include "stb_image_write.h"

static unsigned * render_data   = NULL;
static unsigned   render_width  = 0;
static unsigned   render_height = 0;

static stbtt_fontinfo font[font_types] = { { 0 } };

static signed   font_loaded[font_types]   = { 0 };
static float    font_scale[font_types]    = { 0 };
static signed   font_ascent[font_types]   = { 0 };
static signed   font_descent[font_types]  = { 0 };
static signed   font_line_gap[font_types] = { 0 };
static char   * font_buffer[font_types]   = { NULL };

font_type font_style  = font_normal;
unsigned  font_size   = 24;
unsigned  font_width  = 0;
unsigned  font_height = 0;

unsigned image_limit = 0;
unsigned image_carry = 0;

colour_t render_fg = 0xffffffff;
colour_t render_bg = 0xff000000;
colour_t render_no = 0xff000000;

static unsigned channel_r (unsigned colour) { return ((colour >>  0) & 0xff); }
static unsigned channel_g (unsigned colour) { return ((colour >>  8) & 0xff); }
static unsigned channel_b (unsigned colour) { return ((colour >> 16) & 0xff); }
static unsigned channel_a (unsigned colour) { return ((colour >> 24) & 0xff); }

static colour_t get_colour(unsigned char alpha) {
    double scale = (double) alpha / 255.0;

    if (scale <= 0.0) { return render_bg; }
    if (scale >= 1.0) { return render_fg; }

    unsigned r = (unsigned) ((1.0 - scale) * channel_r (render_bg)
                                  + scale  * channel_r (render_fg));
    unsigned g = (unsigned) ((1.0 - scale) * channel_g (render_bg)
                                  + scale  * channel_g (render_fg));
    unsigned b = (unsigned) ((1.0 - scale) * channel_b (render_bg)
                                  + scale  * channel_b (render_fg));
    unsigned a = (unsigned) ((1.0 - scale) * channel_a (render_bg)
                                  + scale  * channel_a (render_fg));

    return ((r << 0) | (g << 8) | (b << 16) | (a << 24));
}

void render_defaults(unsigned width, unsigned height) {
    width  *= font_width;
    height *= font_height;

    render_data = calloc(width * height, sizeof(*render_data));

    for (unsigned y = 0; y < height; ++y) {
        for (unsigned x = 0; x < width; ++x) {
            render_data[y * width + x] = render_no;
        }
    }

    render_width  = width;
    render_height = height;
}

signed render_character(signed c, unsigned x, unsigned y) {
#define scaling (32)
    unsigned char pixels[scaling * scaling] = { 0 };

    signed advance = 0, lsb = 0, x0 = 0, y0 = 0, x1 = 0, y1 = 0;

    if (!font_loaded[font_style]) {
        font_style = font_normal;
    }

    stbtt_GetCodepointHMetrics(&font[font_style], c, &advance, &lsb);

    stbtt_GetCodepointBitmapBox(&font[font_style], c, font_scale[font_style],
                                font_scale[font_style], &x0, &y0, &x1, &y1);

    signed off = roundf(lsb * font_scale[font_style]) + scaling
               * (font_ascent[font_style] + y0);

    off = (off < 0) ? 0 : off;

    stbtt_MakeCodepointBitmap(&font[font_style], pixels + off, x1 - x0,
                              y1 - y0, scaling, font_scale[font_style],
                              font_scale[font_style], c);

    for (unsigned i = 0; i < font_height; ++i) {
        for (unsigned j = 0; j < font_width; ++j) {
            unsigned data = get_colour(pixels[i * scaling + j]);
            if (render_data[(y + i) * render_width + (x + j)] == render_no) {
                render_data[(y + i) * render_width + (x + j)] = data;
            }
        }
    }

    return roundf(advance * font_scale[font_style]);
#undef scaling
}

signed import_ttf_font(const char * name) {
    FILE * font_file = fopen(name, "rb");

    if (font_loaded[font_style]) {
        fprintf(stderr, "ERROR: You already loaded this font style...\n");
        return 1;
    }

    if (!font_file) {
        fprintf(stderr, "ERROR: Failed to open font file...\n");
        return 1;
    }

    fseek(font_file, 0, SEEK_END);
    long font_size_bytes = ftell(font_file);
    fseek(font_file, 0, SEEK_SET);

    font_buffer[font_style] = malloc(font_size_bytes);
    fread(font_buffer[font_style], 1, font_size_bytes, font_file);
    fclose(font_file);

    if (!stbtt_InitFont(&font[font_style],
                        (unsigned char *)font_buffer[font_style],
                        0)) {
        fprintf(stderr, "ERROR: Failed to initialize font...\n");
        free(font_buffer[font_style]);
        return 1;
    }

    font_scale[font_style] = stbtt_ScaleForPixelHeight(&font[font_style],
                                                       font_size);

    stbtt_GetFontVMetrics(&font[font_style],
                          &font_ascent[font_style],
                          &font_descent[font_style],
                          &font_line_gap[font_style]);

    font_ascent[font_style] = roundf(font_ascent[font_style]
                                     * font_scale[font_style]);

    font_descent[font_style] = roundf(font_descent[font_style]
                                      * font_scale[font_style]);

    for (signed index = 32; index <= font[font_style].numGlyphs; index++) {
        signed advance = 0, lsb = 0, x0 = 0, y0 = 0, x1 = 0, y1 = 0;

        stbtt_GetCodepointHMetrics(&font[font_style], index, &advance, &lsb);

        stbtt_GetCodepointBitmapBox(&font[font_style], index,
                                    font_scale[font_style],
                                    font_scale[font_style],
                                    &x0, &y0, &x1, &y1);

        signed width  = x1 - x0;
        signed height = y1 - y0;

        if (width  > (signed) font_width) {
            font_width = width;
        }
        if (height > (signed) font_height) {
            font_height = height;
        }
    }

    font_loaded[font_style] = !font_loaded[font_style];

    return 0;
}

signed export_png_image(const char * name) {
    if (name == NULL) {
        fprintf (stderr, "ERROR: Name is null...\n");
        return 1;
    }

    unsigned * buffer = malloc(image_limit * render_height * sizeof(*buffer));

    for (unsigned y = 0; y < render_height; ++y) {
        for (unsigned x = 0; x < image_limit; ++x) {
            buffer[y * image_limit + x] = render_data[y * render_width + x];
        }
    }

    stbi_write_png(name, image_limit, render_height, 4, buffer,
                   image_limit * 4);

    for (signed index = 0; index < font_types; ++index) {
        if (font_loaded[index]) {
            free(font_buffer[index]);
        }
    }

    free(render_data);
    free(buffer);

    return 0;
}
