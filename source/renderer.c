#include "renderer.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "error.h"

#define STB_TRUETYPE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb_truetype.h"
#include "stb_image_write.h"

static unsigned * render_data   = NULL;
static unsigned   render_width  = 0;
static unsigned   render_height = 0;

static stbtt_fontinfo font_info[font_types] = { { 0 } };

static float    font_scale[font_types]    = { 0 };
static signed   font_ascent[font_types]   = { 0 };
static signed   font_descent[font_types]  = { 0 };
static signed   font_line_gap[font_types] = { 0 };
static char   * font_buffer[font_types]   = { NULL };

static unsigned          * glyph_count  = NULL;
static signed          * * glyph_index  = NULL;
static unsigned        * * glyph_width  = NULL;
static unsigned        * * glyph_height = NULL;
static unsigned char * * * glyph_data   = NULL;

font_type font_style = font_normal;
unsigned  font_size  = 24;

unsigned font_width[font_types]  = { 0 };
unsigned font_height[font_types] = { 0 };

unsigned image_limit = 0;
unsigned image_carry = 0;

colour_t render_fg = 0xffffffff;
colour_t render_bg = 0xff000000;
colour_t render_no = 0xff000000;

static unsigned channel_r (unsigned colour) { return ((colour >>  0) & 0xff); }
static unsigned channel_g (unsigned colour) { return ((colour >>  8) & 0xff); }
static unsigned channel_b (unsigned colour) { return ((colour >> 16) & 0xff); }
static unsigned channel_a (unsigned colour) { return ((colour >> 24) & 0xff); }

static
colour_t get_colour(unsigned char alpha) {
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
    width  *= font_width[font_style];
    height *= font_height[font_style];

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
    signed cur = 0;

    for (; cur < glyph_count[font_style]; ++cur) {
        if (c == glyph_index[font_style][cur]) break;
    }

    if (cur == glyph_count[font_style]) {
        signed advance = 0, lsb = 0, x0 = 0, y0 = 0, x1 = 0, y1 = 0;

        ++glyph_count[font_style];

        glyph_index[font_style][cur] = c;

        glyph_data[font_style][cur] = calloc(font_size * font_size + font_size,
                                             sizeof(***glyph_data));

        stbtt_GetCodepointHMetrics(&font_info[font_style], c, &advance, &lsb);

        stbtt_GetCodepointBitmapBox(&font_info[font_style], c,
                                    font_scale[font_style],
                                    font_scale[font_style], &x0, &y0, &x1, &y1);

        signed off = roundf(lsb * font_scale[font_style]) + font_size
                   * (font_ascent[font_style] + y0);

        stbtt_MakeCodepointBitmap(&font_info[font_style],
                                  glyph_data[font_style][cur] + off,
                                  x1 - x0, y1 - y0, font_size,
                                  font_scale[font_style],
                                  font_scale[font_style], c);

        glyph_width[font_style][cur] = roundf(advance * font_scale[font_style]);
    }

    for (unsigned i = 0; i < font_height[font_style]; ++i) {
        for (unsigned j = 0; j < font_width[font_style]; ++j) {
            if (render_data[(y + i) * render_width + (x + j)] == render_no) {
                render_data[(y + i) * render_width + (x + j)] =
                    get_colour(glyph_data[font_style][cur][i * font_size + j]);
            }
        }
    }

    return glyph_width[font_style][cur];
}

signed import_ttf_font(const char * name) {
    FILE * font_file = fopen(name, "rb");

    if (!font_file) {
        error("Failed to open font file '%s'.", name);
        return 1;
    }

    fseek(font_file, 0, SEEK_END);
    long font_size_bytes = ftell(font_file);
    fseek(font_file, 0, SEEK_SET);

    font_buffer[font_style] = malloc(font_size_bytes);
    fread(font_buffer[font_style], 1, font_size_bytes, font_file);
    fclose(font_file);

    if (!stbtt_InitFont(&font_info[font_style],
                        (unsigned char *)font_buffer[font_style],
                        0)) {
        error("Failed to initialize font.");
        free(font_buffer[font_style]);
        return 1;
    }

    font_scale[font_style] = stbtt_ScaleForPixelHeight(&font_info[font_style],
                                                       font_size);

    stbtt_GetFontVMetrics(&font_info[font_style],
                          &font_ascent[font_style],
                          &font_descent[font_style],
                          &font_line_gap[font_style]);

    font_ascent[font_style] = roundf(font_ascent[font_style]
                                     * font_scale[font_style]);

    font_descent[font_style] = roundf(font_descent[font_style]
                                      * font_scale[font_style]);

    for (signed glyph = 0; glyph <= font_info[font_style].numGlyphs; ++glyph) {
        signed advance = 0, lsb = 0, x0 = 0, y0 = 0, x1 = 0, y1 = 0;

        stbtt_GetCodepointHMetrics(&font_info[font_style], glyph, &advance, &lsb);

        stbtt_GetCodepointBitmapBox(&font_info[font_style], glyph,
                                    font_scale[font_style],
                                    font_scale[font_style],
                                    &x0, &y0, &x1, &y1);

        signed width  = x1 - x0;
        signed height = y1 - y0;

        if (width > (signed) font_width[font_style]) {
            font_width[font_style] = width;
        }
        if (height > (signed) font_height[font_style]) {
            font_height[font_style] = height;
        }
    }

    if (!glyph_count) {
        glyph_count  = calloc(font_types, sizeof(*glyph_count));
        glyph_index  = calloc(font_types, sizeof(*glyph_index));
        glyph_width  = calloc(font_types, sizeof(*glyph_width));
        glyph_height = calloc(font_types, sizeof(*glyph_height));
        glyph_data   = calloc(font_types, sizeof(*glyph_data));
    }

    unsigned count = font_info[font_style].numGlyphs;

    glyph_index[font_style]  = calloc(count, sizeof(**glyph_index));
    glyph_width[font_style]  = calloc(count, sizeof(**glyph_width));
    glyph_height[font_style] = calloc(count, sizeof(**glyph_height));
    glyph_data[font_style]   = calloc(count, sizeof(**glyph_data));

    return 0;
}

signed export_png_image(const char * name) {
    unsigned * buffer = malloc(image_limit * render_height * sizeof(*buffer));

    for (unsigned y = 0; y < render_height; ++y) {
        for (unsigned x = 0; x < image_limit; ++x) {
            buffer[y * image_limit + x] = render_data[y * render_width + x];
        }
    }

    stbi_write_png(name, image_limit, render_height, 4, buffer,
                   image_limit * 4);

    for (signed font = 0; font < font_types; ++font) {
        for (signed glyph = 0; glyph < glyph_count[font]; ++glyph) {
            free(glyph_data[font][glyph]);
        }

        free(glyph_index[font]);
        free(glyph_width[font]);
        free(glyph_height[font]);
        free(glyph_data[font]);

        free(font_buffer[font]);
    }

    free(glyph_count);
    free(glyph_index);
    free(glyph_width);
    free(glyph_height);
    free(glyph_data);

    free(render_data);
    free(buffer);

    return 0;
}
