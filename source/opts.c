#include "opts.h"

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include "error.h"

static
void usage(void) {
    fputs(
        "xeen [options] < DATA\n"
        "\t-h --help           : print help and exit\n"
        "\t-v --version        : print version and exit\n"
        "\t-o --ouput <file>   : specify output\n"
        "\t-t --tab-size  <n>  : set tab width\n"
        "\t-s --font-size <n>  : set font size\n"
        "\t-f --font <file>    : set font\n"
        "\t-F --font-dir <dir> : set font directory\n"
        , stdout
    );
}

void parse_args(const int argc, const char * const * const argv) {
    int opt;

    opterr = 0; // suppress default getopt error messages

    struct option long_options[] = {
        {"help",      false, NULL, 'h'},
        {"version",   false, NULL, 'v'},
        {"output",    true,  NULL, 'o'},
        {"tab-size",  true,  NULL, 't'},
        {"font-size", true,  NULL, 's'},
        {"font",      true,  NULL, 'f'},
        {"font-dir",  true,  NULL, 'F'},
        {0, 0, 0, 0},
    };

    while ((opt = getopt_long(argc, (char**)argv, "hvo:t:s:f:F:", long_options, NULL)) != -1) {
        switch (opt) {
            case 'h': {
                usage();
                exit(0);
            } break;
            case 'v': {
                fputs(
                    #include "version.inc"
                    , stdout
                );
                fputs(" - ", stdout);
                puts(RENDERER);
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
