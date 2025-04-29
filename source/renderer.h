#ifndef RENDERER_H
#define RENDERER_H

extern void render_create(unsigned width, unsigned height, unsigned indent);
extern void render_delete(void);

extern void render_character(char code, unsigned x, unsigned y, unsigned colour);

extern void render_string(const char * string, unsigned x, unsigned y, unsigned colour);

#endif
