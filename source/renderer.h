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
extern unsigned  font_indent;

extern unsigned  font_width[font_types];
extern unsigned  font_height[font_types];

extern unsigned image_limit;
extern unsigned image_carry;

extern colour_t render_fg;
extern colour_t render_bg;
extern colour_t render_no;

extern signed renderer_init(unsigned width, unsigned height, const char * normal, const char * bold, const char * italic, const char * bold_italic);

extern signed render_character(signed c, unsigned x, unsigned y);

extern signed export_png_image(const char * name);

#endif
