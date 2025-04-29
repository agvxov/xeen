#include "exporter.h"

#include <stdio.h>
#include <png.h>

signed export_png_image(const char     * path,
                        const unsigned * data,
                              unsigned   width,
                              unsigned   height) {
    png_image image = { 0 };

    // We can add checks here to see if file path or data is NULL.
    // if ((width > 4096) || (height > 8192)) {
    //     fprintf(stderr, "What the fuck are you doing boy...?\n");
    //     return 1;
    // }
    // if ((path == NULL) || (data == NULL)) {
    //     fprintf(stderr, "You write evil code!\n");
    //     return 1;
    // }

    image.version = PNG_IMAGE_VERSION;
    image.format  = PNG_FORMAT_RGBA;
    image.width   = width;
    image.height  = height;

    png_image_write_to_file(&image, path, 0, data, 0, NULL);

    png_image_free(&image);

    return 0;
}
