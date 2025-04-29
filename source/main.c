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

signed main(const int argc, const char * const argv[]) {
    parse_args(argc, argv);

    sds input = stdin2str();

    int w, h;
    myyylex((char*)input, sdslen(input), &h, &w);

    /* NOTE: i have no clue how libpng works and how you prefer to use it;
     *        bellow im guessing how it will look, correct me where im wrong
     */

    // calculate dimension
    // create canvas
    // call fill()
    // write to disk
    // free

    set_colour(255, 127, 63);

    unsigned x = 0, y = 0;

    render_create(input, 4);

    printf("Rendering image %i x %i...\n", render_width, render_height);

    printf("%s", input); // ANON

    for (unsigned index = 0; input[index] != '\0'; ++index) {
        if (input[index] == '\t') {
            x += font_width * render_indent;
        } else if (input[index] == '\n') {
            x *= 0;
            y += font_height;
        } else {
            render_character(input[index], x, y);
            x += font_width;
        }
    }

    export_png_image("lol.png", render_data, render_width, render_height);

    render_delete();

    sdsfree(input);

    return 0;
}
