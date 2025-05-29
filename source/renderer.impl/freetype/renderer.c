#include "renderer.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ft2build.h>
#include <freetype/freetype.h>
#include "error.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

font_type font_style = font_normal;
unsigned font_size = 48;
unsigned font_indent = 0;

unsigned font_width[font_types];
unsigned font_height[font_types];

unsigned image_limit = 0;
unsigned image_carry = 0;

colour_t render_fg = 0xffffffff;
colour_t render_bg = 0xff000000;
colour_t render_no = 0xff000000;

static unsigned render_height = 10000;
static unsigned render_width  = 10000;
static unsigned * render_data = NULL;

static FT_Library ft;
static FT_Face faces[font_types];

static
signed import_ttf_font(const char * name) {
    if (FT_New_Face(ft, name, 0, &faces[font_style])) {
        error("Could not load font: '%s'.", name);
        return 1;
    }
    if (FT_Set_Pixel_Sizes(faces[font_style], 0, font_size)) {
        error("Could not set font size.");
        return 1;
    }
    font_width[font_style]  = faces[font_style]->size->metrics.max_advance >> 6;
    font_height[font_style] = faces[font_style]->size->metrics.height >> 6;

    return 0;
}

signed renderer_init(unsigned width, unsigned height, const char * normal, const char * bold, const char * italic, const char * bold_italic) {
  #define CHECKED_LOAD(x) do {     \
    font_style = font_ ## x;       \
    import_ttf_font(x);            \
  } while (0)

    if (FT_Init_FreeType(&ft)) {
        error("Could not initialize FreeType.\n");
        return 1;
    }

    CHECKED_LOAD(normal);
    CHECKED_LOAD(bold);
    CHECKED_LOAD(italic);
    CHECKED_LOAD(bold_italic);
    font_style = font_normal;

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

    return 0;
  #undef CHECKED_LOAD
}

signed render_character(signed c, unsigned x, unsigned y) {
    if (FT_Load_Char(faces[font_style], c, FT_LOAD_RENDER)) {
        error("Could not load character '%c'.", c);
        return 1;
    }

    FT_Bitmap * bmp = &faces[font_style]->glyph->bitmap;

    int x_off = x + faces[font_style]->glyph->bitmap_left;
    int y_off = y - faces[font_style]->glyph->bitmap_top + font_size;

    // Background
    for (int row = 0; row < font_size; row++) {
        for (int col = 0; col < faces[font_style]->glyph->advance.x >> 6; col++) {
            int xi = x + col;
            int yi = y + row;
            if (xi >= 0
            &&  xi < render_width
            &&  yi >= 0
            &&  yi < render_height) {
                render_data[yi * render_width + xi] = render_bg;
            }
        }
    }

    // Character
    for (int row = 0; row < bmp->rows; row++) {
        for (int col = 0; col < bmp->width; col++) {
            int xi = x_off + col;
            int yi = y_off + row;
            if (xi >= 0
            &&  xi < render_width
            &&  yi >= 0
            &&  yi < render_height) {
                unsigned char gray = bmp->buffer[row * bmp->pitch + col];
                unsigned char r = ((render_fg >> 16) & 0xFF) * gray / 255;
                unsigned char g = ((render_fg >> 8)  & 0xFF) * gray / 255;
                unsigned char b = ( render_fg        & 0xFF) * gray / 255;
                unsigned rgb = (r << 16) | (g << 8) | b;

                render_data[yi * render_width + xi] |= rgb;
            }
        }
    }

    return faces[font_style]->glyph->advance.x >> 6;
}

signed export_png_image(const char * name) {
  #define COMPRESSION_LEVEL 4
    int r = 0;

    unsigned * buffer = malloc(image_limit * render_height * sizeof(*buffer));

    for (unsigned y = 0; y < render_height; ++y) {
        for (unsigned x = 0; x < image_limit; ++x) {
            buffer[y * image_limit + x] = render_data[y * render_width + x];
        }
    }

    r = !stbi_write_png(
        name,
        image_limit,
        render_height,
        COMPRESSION_LEVEL,
        buffer,
        image_limit * 4
    );

    free(buffer);

    return r;
  #undef COMPRESSION_LEVEL
}
