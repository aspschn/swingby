#ifndef _FOUNDATION_IMAGE_H
#define _FOUNDATION_IMAGE_H

#include <stdbool.h>
#include <stdint.h>

#include <swingby/size.h>

#ifdef __cplusplus
extern "C" {
#endif

enum sb_image_format {
    SB_IMAGE_FORMAT_RGBA32,
    SB_IMAGE_FORMAT_ARGB32,
};

enum sb_image_file_format {
    SB_IMAGE_FILE_FORMAT_PNG    = 1,
    SB_IMAGE_FILE_FORMAT_JPEG   = 2,
    SB_IMAGE_FILE_FORMAT_AUTO   = 255,
};

typedef struct sb_image_t sb_image_t;

sb_image_t* sb_image_new(const sb_size_i_t *size, enum sb_image_format format);

const sb_size_i_t* sb_image_size(sb_image_t *image);

enum sb_image_format sb_image_format(sb_image_t *image);

uint8_t* sb_image_data(sb_image_t *image);

bool sb_image_load_from_file(sb_image_t *image,
                             const char *filename,
                             enum sb_image_file_format format);

void sb_image_load_from_data(sb_image_t *image,
                             const uint8_t *data,
                             enum sb_image_file_format format);

void sb_image_free(sb_image_t *image);

#ifdef __cplusplus
}
#endif

#endif /* _FOUNDATION_IMAGE_H */
