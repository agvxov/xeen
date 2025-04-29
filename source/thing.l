%{
    #include <stdlib.h>
    #include "renderer.h"

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
%option prefix="thing"
%%
{SCI}   {
            BEGIN IN_CONTROL_SEQUENCE;
        }
\t      {
            x += font_width * render_indent;
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
1       {
            // bold
        }
[0-9]*  {
            int value;
            sscanf(yytext, "%d", value);
            // set renderer
        }
;       { ; }
m       {
            BEGIN INITIAL;
        }
}
%%

int mythinglex(char * str, size_t n) {
    int r;
    input_str = str;
    len       = n;
    offset    = len;

    r = thinglex();

    return r;
}
