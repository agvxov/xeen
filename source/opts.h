#ifndef OPTS_H
#define OPTS_H

extern char * output_filename;
extern char * font_name;
extern char * font_directory;
extern int font_size_opt;
extern int tab_width;

extern void parse_args(const int argc, const char * const * const argv);

#endif
