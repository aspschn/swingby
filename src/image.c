#include <swingby/image.h>

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

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

sb_image_t* sb_image_new2(const sb_image_desc_t *desc)
{
    sb_image_t *image = malloc(sizeof(sb_image_t));

    // Empty.
    if (desc->source_type == SB_IMAGE_SOURCE_TYPE_EMPTY) {
        image->data = malloc(
            sizeof(uint32_t) * (desc->size.width * desc->size.height)
        );
        image->size = desc->size;
    }

    return image;
}

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

const sb_size_i_t* sb_image_size(const sb_image_t *image)
{
    return &image->size;
}

enum sb_image_format sb_image_format(const sb_image_t *image)
{
    return image->format;
}

uint8_t* sb_image_data(sb_image_t *image)
{
    return image->data;
}

void sb_image_set_data(sb_image_t *image,
                       const uint8_t *data,
                       const sb_size_i_t *size)
{
    if (image->data != NULL) {
        free(image->data);
    }
    uint64_t total_size = size->height * size->width * 4;
    image->data = malloc(total_size);
    memcpy(image->data, data, total_size);
    image->size = *size;
}

void sb_image_fill(sb_image_t *image, const sb_color_t *color)
{
    sb_skia_image_fill(image, color);
}

void sb_image_draw_image(sb_image_t *image,
                         const sb_image_t *src,
                         const sb_point_i_t *pos,
                         enum sb_blend_mode blend_mode)
{
    sb_skia_image_draw_image(image, src, pos, blend_mode);
}

bool sb_image_load_from_file(sb_image_t *image,
                             const char *filename,
                             enum sb_image_file_format format)
{
    uint64_t width, height;

    uint8_t *data = sb_skia_load_image_from_file(filename, NULL, 0, format,
        &width, &height);

    if (data == NULL) {
        return false;
    }

    if (image->data != NULL) {
        free(image->data);
        image->data = data;
        image->size.width = width;
        image->size.height = height;
    }

    return true;
}

bool sb_image_load_from_data(sb_image_t *image,
                             const uint8_t *data,
                             uint64_t data_len,
                             enum sb_image_file_format format)
{
    uint64_t width, height;

    uint8_t *ret_data = sb_skia_load_image_from_file(NULL, data, data_len,
        format, &width, &height);

    if (ret_data == NULL) {
        return false;
    }

    if (image->data != NULL) {
        free(image->data);
        image->data = ret_data;
        image->size.width = width;
        image->size.height = height;
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
