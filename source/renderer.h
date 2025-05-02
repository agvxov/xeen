#ifndef RENDERER_H
#define RENDERER_H

typedef unsigned colour_t;

typedef enum {
	font_normal,
	font_bold,
	font_italic,
	font_bold_italic,
	font_types
} font_type;

extern font_type font_style;
extern unsigned  font_size;
extern unsigned  font_width;
extern unsigned  font_height;

extern unsigned image_limit;
extern unsigned image_carry;

extern colour_t render_fg;
extern colour_t render_bg;
extern colour_t render_no;

extern void render_create(unsigned width, unsigned height);

extern signed render_character(signed c, unsigned x, unsigned y);

extern colour_t rgb2colour_t(colour_t red, colour_t green, colour_t blue);

extern signed import_ttf_font(const char * name);
extern signed export_png_image(const char * name);

#endif
