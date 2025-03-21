#include <swingby/image.h>

#include <stdint.h>
#include <stdlib.h>

#include <swingby/log.h>

#include "skia/load-image.h"

#ifdef __cplusplus
extern "C" {
#endif

struct sb_image_t {
    sb_size_i_t size;
    enum sb_image_format format;
    void *data;
};

sb_image_t* sb_image_new(const sb_size_i_t *size, enum sb_image_format format)
{
    sb_image_t *image = malloc(sizeof(sb_image_t));

    if (size->width == 0 || size->height == 0) {
        sb_log_warn("sb_image_new() - Image size is invalid.\n");
    }
    image->size = *size;
    image->format = format;

    image->data = malloc(
        sizeof(uint32_t) * (image->size.width * image->size.height));

    return image;
}

const sb_size_i_t* sb_image_size(sb_image_t *image)
{
    return &image->size;
}

enum sb_image_format sb_image_image_format(sb_image_t *image)
{
    return image->format;
}

uint8_t* sb_image_data(sb_image_t *image)
{
    return image->data;
}

bool sb_image_load_from_file(sb_image_t *image,
                             const char *filename,
                             enum sb_image_file_format format)
{
    uint64_t width, height;

    uint8_t *data = sb_skia_load_image_from_file(filename, format,
        &width, &height);

    if (data == NULL) {
        return false;
    }

    if (image->data != NULL) {
        free(image->data);
        image->data = data;
    }

    return true;
}

void sb_image_free(sb_image_t *image)
{
    free(image->data);
    free(image);
}

#ifdef __cplusplus
}
#endif
