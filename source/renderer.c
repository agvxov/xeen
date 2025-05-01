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

//~static
//~const unsigned long font_glyph[96] = {
    //~0x0000000000000000, 0x00180018183c3c18, 0x0000000000363636,
    //~0x006c6cfe6cfe6c6c, 0x00187ed07c16fc30, 0x0060660c18306606,
    //~0x00dc66b61c36361c, 0x0000000000181818, 0x0030180c0c0c1830,
    //~0x000c18303030180c, 0x0000187e3c7e1800, 0x000018187e181800,
    //~0x0c18180000000000, 0x000000007e000000, 0x0018180000000000,
    //~0x0000060c18306000, 0x003c666e7e76663c, 0x007e181818181c18,
    //~0x007e0c183060663c, 0x003c66603860663c, 0x0030307e363c3830,
    //~0x003c6660603e067e, 0x003c66663e060c38, 0x000c0c0c1830607e,
    //~0x003c66663c66663c, 0x001c30607c66663c, 0x0018180018180000,
    //~0x0c18180018180000, 0x0030180c060c1830, 0x0000007e007e0000,
    //~0x000c18306030180c, 0x001800181830663c, 0x003c06765676663c,
    //~0x006666667e66663c, 0x003e66663e66663e, 0x003c66060606663c,
    //~0x001e36666666361e, 0x007e06063e06067e, 0x000606063e06067e,
    //~0x003c66667606663c, 0x006666667e666666, 0x007e18181818187e,
    //~0x001c36303030307c, 0x0066361e0e1e3666, 0x007e060606060606,
    //~0x00c6c6d6d6feeec6, 0x006666767e6e6666, 0x003c66666666663c,
    //~0x000606063e66663e, 0x006c36566666663c, 0x006666363e66663e,
    //~0x003c66603c06663c, 0x001818181818187e, 0x003c666666666666,
    //~0x00183c6666666666, 0x00c6eefed6d6c6c6, 0x0066663c183c6666,
    //~0x001818183c666666, 0x007e060c1830607e, 0x003e06060606063e,
    //~0x00006030180c0600, 0x007c60606060607c, 0x000000000000663c,
    //~0xffff000000000000, 0x000000000030180c, 0x007c667c603c0000,
    //~0x003e6666663e0606, 0x003c6606663c0000, 0x007c6666667c6060,
    //~0x003c067e663c0000, 0x000c0c0c3e0c0c38, 0x3c607c66667c0000,
    //~0x00666666663e0606, 0x003c1818181c0018, 0x0e181818181c0018,
    //~0x0066361e36660606, 0x003c18181818181c, 0x00c6d6d6fe6c0000,
    //~0x00666666663e0000, 0x003c6666663c0000, 0x06063e66663e0000,
    //~0xe0607c66667c0000, 0x000606066e360000, 0x003e603c067c0000,
    //~0x00380c0c0c3e0c0c, 0x007c666666660000, 0x00183c6666660000,
    //~0x006cfed6d6c60000, 0x00663c183c660000, 0x3c607c6666660000,
    //~0x007e0c18307e0000, 0x003018180e181830, 0x0018181818181818,
    //~0x000c18187018180c, 0x000000000062d68c, 0x0000000000000000
//~};

unsigned font_size   = 18;
//~unsigned font_pad    = 1;
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
    //~width  *= font_pad + font_width;
    //~height *= font_pad + font_height;
    width  *= font_width;
    height *= font_height;
    //~width  *= font_width;
    //~height *= font_height;

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
    //~for (unsigned index = 0; index < font_width * font_height; ++index) {
        //~unsigned u = index / font_width + y;
        //~unsigned v = index % font_width + x;

        //~unsigned c = font_glyph[(unsigned)(code - ' ')] >> index;

        //~render_data[u * render_width + v] = (c & 1)
                                          //~? render_colour
                                          //~: render_empty;
    //~}
    #define scaling (32)

    if ((character <= 31) || (character >= 127)) return;

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
    //~for (int i = 0; i < *offy; ++i) {
        //~for (int j = 0; j < *offx; ++j) {
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

    //~int x0 = 0, y0 = 0, x1 = 0, y1 = 0;

    //~stbtt_GetGlyphBox(&font, '#', &x0, &y0, &x1, &y1);

    //~font_width  = x1 - x0;
    //~font_height = y1 - y0;

    for (signed index = 32; index <= font.numGlyphs; index++) {
        signed advance = 0, lsb = 0, x0 = 0, y0 = 0, x1 = 0, y1 = 0;

        stbtt_GetCodepointHMetrics(&font, index, &advance, &lsb);

        stbtt_GetCodepointBitmapBox(&font, index, font_scale, font_scale, &x0,
                                    &y0, &x1, &y1);

        signed width  = x1 - x0;
        signed height = y1 - y0;

        if (width  > font_width) {
            font_width = width;
            //~printf("Widest: %c (%i)\n", (char)index, index);
        }
        if (height > font_height) {
            font_height = height;
        }
    }

    //~font_width  = font_size;
    //~font_height = font_size;

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
