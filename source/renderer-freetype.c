#define RENDERER_IMPLEMENTATION
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

static inline
colour_t blend_colors(colour_t background, colour_t foreground, colour_t opacity) {
    unsigned char bg_r = (background >> 16) & 0xFF;
    unsigned char bg_g = (background >> 8)  & 0xFF;
    unsigned char bg_b = background & 0xFF;

    unsigned char fg_r = (foreground >> 16) & 0xFF;
    unsigned char fg_g = (foreground >> 8)  & 0xFF;
    unsigned char fg_b = foreground & 0xFF;

    unsigned char out_r = (fg_r * opacity + bg_r * (255 - opacity)) / 255;
    unsigned char out_g = (fg_g * opacity + bg_g * (255 - opacity)) / 255;
    unsigned char out_b = (fg_b * opacity + bg_b * (255 - opacity)) / 255;

    return (0xFF << 24) | (out_r << 16) | (out_g << 8) | out_b;
}

static inline
int ft_px_ceil(FT_Pos v) {
    return (int)((v + 63) >> 6);
}

int terminal_cell_width(FT_Face face) {
    return ft_px_ceil(face->size->metrics.max_advance);
}

int terminal_line_height(FT_Face face) {
    FT_Size_Metrics *m = &face->size->metrics;

    int ascent  = ft_px_ceil( m->ascender);
    int descent = ft_px_ceil(-m->descender);
    int gap     = ft_px_ceil(m->height - m->ascender + m->descender);

    if (gap < 0)
        gap = 0;

    return ascent + descent + gap;
}

static
signed load_ttf_font(const char * path, font_type font_style) {
    int err;
    if (path) {
        err = FT_New_Face(ft, path, 0, &faces[font_style]);
    } else {
        err = FT_New_Memory_Face(
            ft,
            default_fonts[font_style],
            default_font_sizes[font_style],
            0,
            &faces[font_style]
        );
    }

    if (err) {
        if (!path) { path = "(default)"; }
        error("Could not load font: '%s'.", path);
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

signed renderer_init(
  unsigned width,
  unsigned height,
  const char * normal_path,
  const char * bold_path,
  const char * italic_path,
  const char * bold_italic_path
) {
  #define CHECK(x) if (x) { return 1; }
    if (FT_Init_FreeType(&ft)) {
        error("Could not initialize FreeType.\n");
        return 1;
    }

    CHECK(load_ttf_font(normal_path, font_normal));
    CHECK(load_ttf_font(bold_path, font_bold));
    CHECK(load_ttf_font(italic_path, font_italic));
    CHECK(load_ttf_font(bold_italic_path, font_bold_italic));

    font_style = font_normal;

    font_indent = terminal_cell_width(faces[0]);
    font_size   = terminal_line_height(faces[0]);

    width  *= font_indent;
    height *= font_size;

    render_data = calloc(width * height, sizeof(*render_data));

    for (unsigned y = 0; y < height; ++y) {
        for (unsigned x = 0; x < width; ++x) {
            render_data[y * width + x] = render_no;
        }
    }

    render_width  = width;
    render_height = height;

    return 0;
  #undef CHECK
}

signed render_character(signed c, unsigned x, unsigned y) {
    FT_Face face = faces[font_style];

    if (FT_Load_Char(face, (FT_ULong)c, FT_LOAD_RENDER)) {
        error("Could not load character '%c'.", c);
        return 1;
    }

    FT_GlyphSlot glyph = face->glyph;
    FT_Size_Metrics *m = &face->size->metrics;

    const int cell_width  = font_indent;
    const int line_height = font_size;

    const int ascent = ft_px_ceil(m->ascender);
    const int baseline_y = y + ascent;

    const int x_off = x + glyph->bitmap_left;
    const int y_off = baseline_y - glyph->bitmap_top;

    // Background
    for (int row = 0; row < line_height; row++) {
        for (int col = 0; col < cell_width; col++) {
            int xi = x + col;
            int yi = y + row;

            if (xi >= 0 && xi < render_width &&
                yi >= 0 && yi < render_height) {
                render_data[yi * render_width + xi] = render_bg;
            }
        }
    }

    // Character
    for (unsigned row = 0; row < glyph->bitmap.rows; row++) {
        for (unsigned col = 0; col < glyph->bitmap.width; col++) {
            int xi = x_off + (int)col;
            int yi = y_off + (int)row;

            if (xi >= 0 && xi < render_width &&
                yi >= 0 && yi < render_height) {
                unsigned char gray = glyph->bitmap.buffer[row * glyph->bitmap.pitch + col];
                render_data[yi * render_width + xi] = blend_colors(render_bg, render_fg, gray);
            }
        }
    }

    return cell_width;
}

signed export_png_image(const char * name) {
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
        PNG_COMPRESSION_LEVEL,
        buffer,
        image_limit * 4
    );

    free(buffer);

    return r;
}
