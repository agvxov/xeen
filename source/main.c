#include <stdio.h>
#include "sds.h"
#include "opts.h"
#include "renderer.h"
#include "exporter.h"

// fill()
// for input
//  if escape then set formatting flag
//  else render character to canvas

char * output_filename = "current_thing.png"; // XXX change once we have a name
int tab_width = 8;

#define READ_BATCH_SIZE 64

sds stdin2str(void) {
    sds r = sdsnew("");

    char buffer[READ_BATCH_SIZE + 1];
    int read_count = 0;
    do {
        read_count = fread(buffer, 1, READ_BATCH_SIZE, stdin);
        r = sdscatlen(r, buffer, read_count);
    } while (read_count == READ_BATCH_SIZE);

    return r;
}

extern int myyylex(char * str, size_t n, int * h, int * w);
extern int mythinglex(char * str, size_t n);

signed main(const int argc, const char * const argv[]) {
    parse_args(argc, argv);

    sds input = stdin2str();
    size_t input_len = sdslen(input);

    int w, h;
    myyylex((char*)input, input_len, &h, &w);

    set_colour(255, 127, 63);

    render_create(input, w, h);

    printf("Rendering image %i x %i...\n", render_width, render_height);

    mythinglex(input, input_len);

    export_png_image("lol.png", render_data, render_width, render_height);

    render_delete();

    sdsfree(input);

    return 0;
}
