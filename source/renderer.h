#ifndef RENDERER_H
#define RENDERER_H

#define font_width  (8)
#define font_height (8)

extern unsigned * render_data;
extern unsigned   render_width;
extern unsigned   render_height;
extern unsigned   render_indent;

extern void render_create(const char * file, unsigned indent);
extern void render_delete(void);

extern void render_character(char code, unsigned x, unsigned y);

extern void render_string(const char * string, unsigned x, unsigned y);

extern void set_colour(unsigned red, unsigned green, unsigned blue);

#endif
