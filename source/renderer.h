#ifndef RENDERER_H
#define RENDERER_H

#define font_width  (8)
#define font_height (8)

typedef unsigned colour_t;

extern colour_t render_colour;

extern void render_create(unsigned width, unsigned height);
extern void render_delete(void);

extern void render_character(char character, unsigned x, unsigned y,
                             signed * offx, signed * offy);

extern colour_t rgb2colour_t(colour_t red, colour_t green, colour_t blue);

extern signed import_ttf_font(const char * name);
extern signed export_png_image(const char * name);

#endif
