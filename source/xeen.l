%{
    #include <stdlib.h>
    #include "renderer.h"
    #include "opts.h"
    #include "colorscheme.h"
    #include "str2codepoint.h"

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

    extern colour_t to_bright(colour_t c);
    extern colour_t to_faint(colour_t c);
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
                x += font_indent * tab_width;
            }
.           {
                int offx = render_character(yytext[0], x, y);
                x += offx;
                image_carry += offx;
            }
{utfchar}   {
                int offx = render_character(str2codepoint(yytext), x, y);
                x += offx;
                image_carry += offx;
            }
\n          {
                x *= 0;
                y += font_size;
                image_limit = (image_carry > image_limit)
                            ? image_carry
                            : image_limit;
                image_carry = 0;
            }

<IN_CONTROL_SEQUENCE>{
                    /* --- SPECIAL ---- */
0                   { // reset
                        render_fg  = default_color;
                        render_bg  = default_background;
                        font_style = font_normal;
                    }
1                   { // bold
                        switch (font_style) {
                            case font_bold_italic:
                            case font_italic: font_style = font_bold_italic; break;
                            default:          font_style = font_bold;        break;
                        }
                    }
2                   { // faint
                        render_fg = to_faint(render_fg);
                    }
3                   { // italic
                        switch (font_style) {
                            case font_bold_italic:
                            case font_bold: font_style = font_bold_italic; break;
                            default:        font_style = font_italic;      break;
                        }
                    }
7                   { // reversed
                        colour_t swap;
                        swap = render_fg;
                        render_fg = render_bg;
                        render_bg = swap;
                    }
                    /* --- 4 BIT COLORS --- */
3[0-7]              { // foreground
                        int value;
                        sscanf(yytext, "%d", &value);
                        render_fg = colorscheme[value-30];
                    }
4[0-7]              { // background
                        int value;
                        sscanf(yytext, "%d", &value);
                        render_bg = colorscheme[value-40];
                    }
9[0-7]              { // bright
                        int value;
                        sscanf(yytext, "%d", &value);
                        render_fg = to_bright(colorscheme[value-30]);
                    }
10[0-7]             { // bright background
                        int value;
                        sscanf(yytext, "%d", &value);
                        render_bg = to_bright(colorscheme[value-40]);
                    }
                    /* --- 8 BIT COLORS --- */
38;5;{d}+           {
                        int value;
                        sscanf(yytext+(sizeof("38;5;")-1), "%d", &value);
                        render_fg = colorscheme[value];
                    }
48;5;{d}+           {
                        int value;
                        sscanf(yytext+(sizeof("48;5;")-1), "%d", &value);
                        render_bg = colorscheme[value];
                    }
                    /* --- 24 BIT COLOR --- */
38;2;{d}*;{d}*;{d}* {
                        int r, g, b;
                        sscanf(yytext+(sizeof("38;2;")-1), "%d;%d;%d", &r, &g, &b);
                        render_fg = rgb2colour_t(r, g, b);
                    }
48;2;{d}*;{d}*;{d}* {
                        int r, g, b;
                        sscanf(yytext+(sizeof("48;2;")-1), "%d;%d;%d", &r, &g, &b);
                        render_bg = rgb2colour_t(r, g, b);
                    }
                    /* --- TERMINATORS --- */
;                   { ; }
m                   {
                        BEGIN INITIAL;
                    }
{d}+                {
                        fputs("Unknown escape sequence encountered.\n", stderr);
                    }
.|\n                {
                        fputs("Errorous escape encountered.\n", stderr);
                    }
}
%%

int xeen(char * str, size_t n) {
    int r;
    input_str = str;
    len       = n;
    offset    = len;

    r = xeenlex();

    yy_delete_buffer(YY_CURRENT_BUFFER);

    return r;
}
