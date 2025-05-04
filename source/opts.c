#include "opts.h"

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include "error.h"

static
void usage(void) {
    puts(
        "xeen [options]\n"
        "\t-h        : print help and exit\n"
        "\t-v        : print version and exit\n"
        "\t-o <file> : specify output\n"
        "\t-t <n>    : set tab width\n"
        "\t-s <n>    : set font size\n"
        "\t-f <file> : set font\n"
        "\t-F <file> : set font directory\n"
    );
}

void parse_args(const int argc, const char * const * const argv) {
    int opt;

    opterr = 0; // suppress default getopt error messages

    while ((opt = getopt(argc, (char**)argv, "hvo:t:s:f:F:")) != -1) {
        switch (opt) {
            case 'h': {
                usage();
                exit(0);
            } break;
            case 'v': {
                puts(
                    #include "version.inc"
                );
                exit(0);
            } break;
            case 'o': {
                output_filename = optarg;
            } break;
            case 't': {
                int e = sscanf(optarg, "%d", &tab_width);
                if (!e) {
                    error("Invalid tab width '%s'.", optarg);
                    exit(1);
                }
            } break;
            case 's': {
                int e = sscanf(optarg, "%d", &font_size_opt);
                if (!e) {
                    error("Invalid font size '%s'.", optarg);
                    exit(1);
                }
            } break;
            case 'f': {
                font_name = optarg;
            } break;
            case 'F': {
                font_directory = optarg;
            } break;
            case '?': {
                error("Unknown argument '-%c'.", optopt);
                exit(1);
            }
        }
    }
}
