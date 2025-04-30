%{
    #include <stdlib.h>
    #include "opts.h"
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
%}

CSI     \033\[

%x IN_CONTROL_SEQUENCE

%option noyywrap nodefault
%option nounput
%option prefix="xeen"
%%
[\xC0-\xFD]|[\x80-\xBF] { /* XXX FORCEFUL IGNOREING OF UTF*/; }

{CSI}   {
            BEGIN IN_CONTROL_SEQUENCE;
        }
\t      {
            x += font_width * tab_width;
        }
.       {
            render_character(yytext[0], x, y);
            x += font_width;
        }
\n      {
            x *= 0;
            y += font_height;
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
38;5;\d+            {
                        int value;
                        sscanf(yytext+(sizeof("38;5;")-1), "%d", &value);
                        render_colour = colorscheme[value];
                    }
48;5;\d+            {
                        int value;
                        sscanf(yytext+(sizeof("48;5;")-1), "%d", &value);
                        //render_background = colorscheme[value];
                    }
                    /* --- 24 BIT COLOR --- */
38;2;\d*;\d*;\d*    {
                        int r, g, b;
                        sscanf(yytext+(sizeof("38;2;")-1), "%d;%d;%d", &r, &g, &b);
                        render_colour = rgb2colour_t(r, g, b);
                    }
48;2;\d*;\d*;\d*    {
                        int r, g, b;
                        sscanf(yytext+(sizeof("48;2;")-1), "%d;%d;%d", &r, &g, &b);
                        //render_background = rgb2colour_t(r, g, b);
                    }
                    /* --- TERMINATORS --- */
;                   { ; }
m                   {
                        BEGIN INITIAL;
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
