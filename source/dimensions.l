%{
    #include <stdlib.h>
    #include "opts.h"

    static char * input_str;
    static size_t len;
    static size_t offset;
    #define YY_INPUT(buf, result, max_size) {                        \
        int cpi = (offset && offset > max_size) ? max_size : offset; \
        memcpy(buf, input_str+(len-offset), cpi);                    \
        result = cpi;                                                \
        offset = (cpi > offset) ? 0 : offset - cpi;                  \
    }

    int yycolno    = 0;
    int line_width = 0;

    static inline
    void register_line(void) {
        if (yycolno < line_width) {
            yycolno = line_width;
        }
        line_width = 0;
    }
%}
escape_sequence \033\[[0-9;]+m
%option noyywrap nodefault
%option yylineno
%%
{escape_sequence}   { ; }
.                   {
                        ++line_width;
                    }
\t                  {
                        line_width += tab_width;
                    }
\n                  {
                        register_line();
                    }
<<EOF>>             {
                        register_line();
                        return 0;
                    }
%%

int get_dimensions(char * str, size_t n, int * h, int * w) {
    int r;
    input_str = str;
    len       = n;
    offset    = len;

    r = yylex();

    *h = yylineno;
    *w = yycolno;

    return r;
}
