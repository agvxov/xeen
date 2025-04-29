#include <stdio.h>
#include "sds.h"
#include "opts.h"

// fill()
// for input
//  if escape then set formatting flag
//  else render character to canvas

char * output_filename = "current_thing.png"; // XXX change once we have a name

#define READ_BATCH_SIZE 64

sds stdin2str(void) {
    sds r = sdsnew("");

    char buffer[READ_BATCH_SIZE + 1];
    buffer[READ_BATCH_SIZE] = '\0';
    int read_count = 0;
    do {
        read_count = fread(buffer, 1, READ_BATCH_SIZE, stdin);
        r = sdscat(r, buffer);
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

    sdsfree(input);

    return 0;
}
