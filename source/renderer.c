#include "renderer.h"

#include <stdlib.h>

       colour_t render_colour = 0xff000000;
static unsigned render_empty  = 0xff000000;

// Passed the limit...
static
const unsigned long font_glyph[96] = {
    0x0000000000000000, 0x00180018183c3c18, 0x0000000000363636,
    0x006c6cfe6cfe6c6c, 0x00187ed07c16fc30, 0x0060660c18306606,
    0x00dc66b61c36361c, 0x0000000000181818, 0x0030180c0c0c1830,
    0x000c18303030180c, 0x0000187e3c7e1800, 0x000018187e181800,
    0x0c18180000000000, 0x000000007e000000, 0x0018180000000000,
    0x0000060c18306000, 0x003c666e7e76663c, 0x007e181818181c18,
    0x007e0c183060663c, 0x003c66603860663c, 0x0030307e363c3830,
    0x003c6660603e067e, 0x003c66663e060c38, 0x000c0c0c1830607e,
    0x003c66663c66663c, 0x001c30607c66663c, 0x0018180018180000,
    0x0c18180018180000, 0x0030180c060c1830, 0x0000007e007e0000,
    0x000c18306030180c, 0x001800181830663c, 0x003c06765676663c,
    0x006666667e66663c, 0x003e66663e66663e, 0x003c66060606663c,
    0x001e36666666361e, 0x007e06063e06067e, 0x000606063e06067e,
    0x003c66667606663c, 0x006666667e666666, 0x007e18181818187e,
    0x001c36303030307c, 0x0066361e0e1e3666, 0x007e060606060606,
    0x00c6c6d6d6feeec6, 0x006666767e6e6666, 0x003c66666666663c,
    0x000606063e66663e, 0x006c36566666663c, 0x006666363e66663e,
    0x003c66603c06663c, 0x001818181818187e, 0x003c666666666666,
    0x00183c6666666666, 0x00c6eefed6d6c6c6, 0x0066663c183c6666,
    0x001818183c666666, 0x007e060c1830607e, 0x003e06060606063e,
    0x00006030180c0600, 0x007c60606060607c, 0x000000000000663c,
    0xffff000000000000, 0x000000000030180c, 0x007c667c603c0000,
    0x003e6666663e0606, 0x003c6606663c0000, 0x007c6666667c6060,
    0x003c067e663c0000, 0x000c0c0c3e0c0c38, 0x3c607c66667c0000,
    0x00666666663e0606, 0x003c1818181c0018, 0x0e181818181c0018,
    0x0066361e36660606, 0x003c18181818181c, 0x00c6d6d6fe6c0000,
    0x00666666663e0000, 0x003c6666663c0000, 0x06063e66663e0000,
    0xe0607c66667c0000, 0x000606066e360000, 0x003e603c067c0000,
    0x00380c0c0c3e0c0c, 0x007c666666660000, 0x00183c6666660000,
    0x006cfed6d6c60000, 0x00663c183c660000, 0x3c607c6666660000,
    0x007e0c18307e0000, 0x003018180e181830, 0x0018181818181818,
    0x000c18187018180c, 0x000000000062d68c, 0x0000000000000000
};

unsigned * render_data   = NULL;
unsigned   render_width  = 0;
unsigned   render_height = 0;
unsigned   render_indent = 4;

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

void render_character(char code, unsigned x, unsigned y) {
    for (unsigned index = 0; index < font_width * font_height; ++index) {
        unsigned u = index / font_width + y;
        unsigned v = index % font_width + x;

        unsigned c = font_glyph[(unsigned)(code - ' ')] >> index;

        render_data[u * render_width + v] = (c & 1)
                                          ? render_colour
                                          : render_empty;
    }
}

void render_string(const char * string, unsigned x, unsigned y) {
    for (unsigned index = 0; string[index] != '\0'; ++index) {
        if ((string[index] >= (char)0) && (string[index] <= (char)31)) {
            // I can't handle X and Y here on new line and tabulator.
            // It requires changing the interface or making them variables.
            return;
        }

        render_character(string[index], x, y);
    }
    // You want to add to Y 'font_height' plus padding if you want on new line.
    // And you want to add to X 'strlen' of string, and some N for tabulator.
    // Do that only after this function finishes, otherwise there'll be offset.
}

colour_t rgb2colour_t(colour_t red, colour_t green, colour_t blue) {
    colour_t r;
    r = 0xff000000 | (blue << 16) | (green << 8) | (red << 0);
    return r;
}
