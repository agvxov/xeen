%{
    #include <stdlib.h>
    #include "renderer.h"
    #include "colorscheme.h"

    static char * input_str;
    static size_t len;
    static size_t offset;
    #define YY_INPUT(buf, result, max_size) {                        \
        int cpi = (offset && offset > max_size) ? max_size : offset; \
        memcpy(buf, input_str+(len-offset), cpi);                    \
        result = cpi;                                                \
        offset = (cpi > offset) ? 0 : offset - cpi;                  \
    }

    size_t x = 0;
    size_t y = 0;

    signed utf8_to_codepoint(const char *s) {
        const unsigned char *bytes = (const unsigned char *)s;
        signed codepoint = 0;

        if (bytes[0] < 0x80) {
            // 1-byte (ASCII)
            codepoint = bytes[0];
        } else if ((bytes[0] & 0xE0) == 0xC0) {
            // 2-byte
            codepoint = ((bytes[0] & 0x1F) << 6) |
                        (bytes[1] & 0x3F);
        } else if ((bytes[0] & 0xF0) == 0xE0) {
            // 3-byte
            codepoint = ((bytes[0] & 0x0F) << 12) |
                        ((bytes[1] & 0x3F) << 6) |
                        (bytes[2] & 0x3F);
        } else if ((bytes[0] & 0xF8) == 0xF0) {
            // 4-byte
            codepoint = ((bytes[0] & 0x07) << 18) |
                        ((bytes[1] & 0x3F) << 12) |
                        ((bytes[2] & 0x3F) << 6) |
                        (bytes[3] & 0x3F);
        }

        return codepoint;
    }


#define RUNE_C(x) INT32_C(x)
typedef int32_t Rune;

#define UTFmax 6 /* maximum bytes per rune */

#define Runeerror ((Rune)-1)         /* decoding error in utf */

int
isvalidrune(Rune r)
{
	if(r < 0)
		return 0; /* negative value */

	if(r >= 0xD800 && r <= 0xDFFF)
		return 0; /* surrogate pair range */

	if(r >= 0xFDD0 && r <= 0xFDEF)
		return 0; /* non-character range */

	if((r & 0xFFFE) == 0xFFFE)
		return 0; /* non-character at end of plane */

	if(r > 0x10FFFF)
		return 0; /* too large, thanks to UTF-16 */

	return 1;
}

const unsigned char utftab[64] = {
	0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, /* 1100xxxx */
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, /* 1101xxxx */
	3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, /* 1110xxxx */
	4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 0, 0, /* 1111xxxx */
};

int
charntorune(Rune *p, const char *s, size_t len)
{
	unsigned char c, i, m, n, x;
	Rune r;

	if(len == 0) /* can't even look at s[0] */
		return 0;

	c = *s++;

	if(!(c & 0200)) /* basic byte */
		return (*p = c, 1);

	if(!(c & 0100)) /* continuation byte */
		return (*p = Runeerror, 1);

	n = utftab[c & 077];

	if(n == 0) /* illegal byte */
		return (*p = Runeerror, 1);

	if(len == 1) /* reached len limit */
		return 0;

	if((*s & 0300) != 0200) /* not a continuation byte */
		return (*p = Runeerror, 1);

	x = 0377 >> n;
	r = c & x;

	r = (r << 6) | (*s++ & 077);

	if(r <= x) /* overlong sequence */
		return (*p = Runeerror, 2);

	m = (len < n) ? len : n;

	for(i = 2; i < m; i++) {
		if((*s & 0300) != 0200) /* not a continuation byte */
			return (*p = Runeerror, i);

		r = (r << 6) | (*s++ & 077);
	}

	if(i < n) /* must have reached len limit */
		return 0;

	if(!isvalidrune(r))
		return (*p = Runeerror, i);

	return (*p = r, i);
}

int
chartorune(Rune *p, const char *s)
{
	return charntorune(p, s, UTFmax);
}
%}

CSI             \033\[
d               [[:digit:]]
utfdatabyte     [\x80-\xBF]
utf2byte        [\xC0-\xDF]{utfdatabyte}
utf3byte        [\xE0-\xEF]{utfdatabyte}{utfdatabyte}
utf4byte        [\xF0-\xFF]{utfdatabyte}{utfdatabyte}{utfdatabyte}
utfchar         {utf2byte}|{utf3byte}|{utf4byte}

%x IN_CONTROL_SEQUENCE

%option noyywrap nodefault
%option nounput
%option prefix="xeen"
%%
{CSI}       {
                BEGIN IN_CONTROL_SEQUENCE;
            }
\t          {
                x += font_width * (font_indent - x % font_indent);
            }
.           {
                int offx = 0, offy = 0;
                render_character(yytext[0], x, y, &offx, &offy);
                x += offx;
                image_carry += offx;
            }
{utfchar}   {
                Rune r;
                chartorune(&r, yytext);

                printf("UTF: %s %04X\n", yytext, r);

                int offx = 0, offy = 0;
                render_character(r, x, y, &offx, &offy);
                x += offx;
                image_carry += offx;
            }
\n          {
                x *= 0;
                y += font_height;
                image_limit = (image_carry > image_limit)
                            ? image_carry
                            : image_limit;
                image_carry = 0;
            }

<IN_CONTROL_SEQUENCE>{
                    /* --- SPECIAL ---- */
0                   {
                        render_colour = default_color;
                        //render_background = default_background;
                    }
1                   {
                        // bold
                    }
2                   {
                        // faint (probably mess with the oppacity)
                    }
                    /* --- 4 BIT COLORS --- */
3[0-7]              {
                        int value;
                        sscanf(yytext, "%d", &value);
                        render_colour = colorscheme[value-30];
                    }
4[0-7]              {
                        int value;
                        sscanf(yytext, "%d", &value);
                        //render_background = colorscheme[value-30];
                    }
                        /* NOTE: the below should bright variants, but im color blind */
9[0-7]              {
                        int value;
                        sscanf(yytext, "%d", &value);
                        render_colour = colorscheme[value-30];
                    }
10[0-7]             {
                        int value;
                        sscanf(yytext, "%d", &value);
                        //render_background = colorscheme[value-30];
                    }
                    /* --- 8 BIT COLORS --- */
38;5;{d}+           {
                        int value;
                        sscanf(yytext+(sizeof("38;5;")-1), "%d", &value);
                        render_colour = colorscheme[value];
                    }
48;5;{d}+           {
                        int value;
                        sscanf(yytext+(sizeof("48;5;")-1), "%d", &value);
                        //render_background = colorscheme[value];
                    }
                    /* --- 24 BIT COLOR --- */
38;2;{d}*;{d}*;{d}* {
                        int r, g, b;
                        sscanf(yytext+(sizeof("38;2;")-1), "%d;%d;%d", &r, &g, &b);
                        render_colour = rgb2colour_t(r, g, b);
                    }
48;2;{d}*;{d}*;{d}* {
                        int r, g, b;
                        sscanf(yytext+(sizeof("48;2;")-1), "%d;%d;%d", &r, &g, &b);
                        //render_background = rgb2colour_t(r, g, b);
                    }
                    /* --- TERMINATORS --- */
;                   { ; }
m                   {
                        BEGIN INITIAL;
                    }
.|\n                {
                        fputs("Unknown escape sequence encountered.\n", stderr);
                        return 0;
                    }
}
%%

int xeen(char * str, size_t n) {
    int r;
    input_str = str;
    len       = n;
    offset    = len;

    r = xeenlex();

    return r;
}
