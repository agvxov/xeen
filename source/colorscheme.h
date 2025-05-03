#ifndef COLORSCHEME_H
#define COLORSCHEME_H

#include "renderer.h"

extern const colour_t default_color;
extern const colour_t default_background;
extern colour_t colorscheme[256];

extern colour_t rgb2colour_t(colour_t red, colour_t green, colour_t blue);

#endif
