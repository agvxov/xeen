#include "opts.h"
#include "renderer.h"

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

static
void usage(void) {
    puts(
        "??? [options]\n"
        "\t-v        : print version and exit\n"
        "\t-h        : print help and exit\n"
        "\t-o <file> : specify output"
    );
}

void parse_args(const int argc, const char * const * const argv) {
    int opt;

    opterr = 0; // suppress default getopt error messages

    while ((opt = getopt(argc, (char**)argv, "hvo:t:s:")) != -1) {
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
                    fprintf(stderr, "Invalid tab width '%s'.\n", optarg);
                    exit(1);
                }
            } break;
            case 's': {
                int e = sscanf(optarg, "%d", &font_size);
                if (!e) {
                    fprintf(stderr, "Invalid font size '%s'.\n", optarg);
                    exit(1);
                }
            } break;
            case '?': {
                fprintf(stderr, "Unknown argument '-%c'.\n", optopt);
                exit(1);
            }
        }
    }
}
