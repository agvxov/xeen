#ifndef RENDERER_H
#define RENDERER_H

#define font_width  (8)
#define font_height (8)

typedef unsigned colour_t;

extern unsigned * render_data;
extern unsigned   render_width;
extern unsigned   render_height;
extern unsigned   render_indent;
extern colour_t   render_colour;

colour_t rgb2colour_t(colour_t red, colour_t green, colour_t blue);

extern void render_create(unsigned width, unsigned height);
extern void render_delete(void);

extern void render_character(char code, unsigned x, unsigned y);

extern void render_string(const char * string, unsigned x, unsigned y);

#endif
