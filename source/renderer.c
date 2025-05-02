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
static colour_t   render_empty  = 0xff000000;

static stbtt_fontinfo font = { 0 };

static float  font_scale    = 0;
static signed font_ascent   = 0;
static signed font_descent  = 0;
static signed font_line_gap = 0;

unsigned font_size   = 18;
unsigned font_width  = 0;
unsigned font_height = 0;
unsigned font_indent = 4;

colour_t render_colour = 0xff000000;

static colour_t get_colour(unsigned char alpha) {
    colour_t r = (((render_colour >>  0) & 0xff) * alpha) / 255;
    colour_t g = (((render_colour >>  8) & 0xff) * alpha) / 255;
    colour_t b = (((render_colour >> 16) & 0xff) * alpha) / 255;

    return 0xff000000 | (b << 16) | (g << 8) | (r << 0);
}

void render_create(unsigned width, unsigned height) {
    width  *= font_width;
    height *= font_height;

    render_data = calloc(width * height, sizeof(*render_data));

    for (unsigned y = 0; y < height; ++y) {
        for (unsigned x = 0; x < width; ++x) {
            render_data[y * width + x] = render_empty;
        }
    }

    render_width  = width;
    render_height = height;
}

void render_delete(void) {
    free(render_data);
}

void render_character(char character, unsigned x, unsigned y, signed * offx,
                      signed * offy) {
    #define scaling (32)

    if ((character < 32) || (character > font.numGlyphs)) return;

    unsigned char pixels[scaling*scaling] = { 0 };

    signed advance = 0, lsb = 0, x0 = 0, y0 = 0, x1 = 0, y1 = 0;

    stbtt_GetCodepointHMetrics(&font, character, &advance, &lsb);

    stbtt_GetCodepointBitmapBox(&font, character, font_scale, font_scale, &x0,
                                &y0, &x1, &y1);

    signed off = roundf(lsb * font_scale) + (font_ascent + y0) * scaling;

    off = (off < 0) ? 0 : off;

    *offx = x1 - x0;
    *offy = y1 - y0;

    stbtt_MakeCodepointBitmap(&font, pixels + off, x1 - x0, y1 - y0, scaling,
                              font_scale, font_scale, character);

    for (int i = 0; i < scaling; ++i) {
        for (int j = 0; j < scaling; ++j) {
            unsigned data = get_colour(pixels[i * scaling + j]);
            if (render_data[(y + i) * render_width + (x + j)] == render_empty) {
                render_data[(y + i) * render_width + (x + j)] = data;
            }
        }
    }

    *offx = roundf(advance * font_scale);

    #undef scaling
}

colour_t rgb2colour_t(colour_t red, colour_t green, colour_t blue) {
    colour_t r;
    r = 0xff000000 | (blue << 16) | (green << 8) | (red << 0);
    return r;
}

signed import_ttf_font(const char * name) {
    FILE * font_file = fopen(name, "rb");

    if (!font_file) {
        // We need good error messages here...
        // Should we abort the program, or use fallback font?
        fprintf(stderr, "ERROR: Failed to open font file...\n");
        return 1;
    }

    fseek(font_file, 0, SEEK_END);
    long font_size_bytes = ftell(font_file);
    // Check -1 error code or use size_t without giving a fuck?
    fseek(font_file, 0, SEEK_SET);

    unsigned char* font_buffer = (unsigned char*)malloc(font_size_bytes);
    fread(font_buffer, 1, font_size_bytes, font_file);
    fclose(font_file);

    if (!stbtt_InitFont(&font, font_buffer, 0)) {
        fprintf(stderr, "ERROR: Failed to initialize font...\n");
        free(font_buffer);
        return 1;
    }

    font_scale = stbtt_ScaleForPixelHeight(&font, font_size);

    stbtt_GetFontVMetrics(&font, &font_ascent, &font_descent, &font_line_gap);

    font_ascent = roundf(font_ascent * font_scale);
    font_descent = roundf(font_descent * font_scale);

    for (signed index = 32; index <= font.numGlyphs; index++) {
        signed advance = 0, lsb = 0, x0 = 0, y0 = 0, x1 = 0, y1 = 0;

        stbtt_GetCodepointHMetrics(&font, index, &advance, &lsb);

        stbtt_GetCodepointBitmapBox(&font, index, font_scale, font_scale, &x0,
                                    &y0, &x1, &y1);

        signed width  = x1 - x0;
        signed height = y1 - y0;

        if (width  > font_width) {
            font_width = width;
        }
        if (height > font_height) {
            font_height = height;
        }
    }

    return 0;
    // I'm not cleaning anything yet, this leaks memory.
    // Once we see shit works, we can sanitize it.
    // Please don't remove this comment until stuff renders.
}

signed export_png_image(const char * name) {
    if (name == NULL) {
        // Write proper error message?
        fprintf (stderr, "ERROR: Name is null...\n");
        return 1;
    }

    stbi_write_png(name, render_width, render_height, 4, render_data,
                   render_width * 4);

    return 0;
}
