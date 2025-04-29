#include <stdio.h>
#include <dictate.h>    // XXX remove after the development phase is done
#include "opts.h"

// fill()
// for input
//  if escape then set formatting flag
//  else render character to canvas

char * output_filename = "current_thing.png"; // XXX change once we have a name

signed main(const int argc, const char * const argv[]) {
    parse_args(argc, argv);

    /* NOTE: i have no clue how libpng works and how you prefer to use it;
     *        bellow im guessing how it will look, correct me where im wrong
     */

    // calculate dimension
    // create canvas
    // call fill()
    // write to disk
    // free

    return 0;
}
