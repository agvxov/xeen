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

/* Reads the default font on NULL argument.
 */
extern signed renderer_init(unsigned width, unsigned height, const char * normal_path, const char * bold_path, const char * italic_path, const char * bold_italic_path);

extern signed render_character(signed c, unsigned x, unsigned y);

extern signed export_png_image(const char * name);

#endif

#ifdef RENDERER_IMPLEMENTATION
unsigned char default_font[] = {
    #embed "DejaVuSansMono.ttf"
};
unsigned char default_font_italic[] = {
    #embed "DejaVuSansMono-Oblique.ttf"
};
unsigned char default_font_bold[] = {
    #embed "DejaVuSansMono-Bold.ttf"
};
unsigned char default_font_bolditalic[] = {
    #embed "DejaVuSansMono-BoldOblique.ttf"
};
const unsigned char * const default_fonts[] = {
    (const unsigned char*const)default_font,
    (const unsigned char*const)default_font_bold,
    (const unsigned char*const)default_font_italic,
    (const unsigned char*const)default_font_bolditalic,
};
const long unsigned default_font_sizes[] = {
    sizeof(default_font),
    sizeof(default_font_bold),
    sizeof(default_font_italic),
    sizeof(default_font_bolditalic),
};
#endif
