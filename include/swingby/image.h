#ifndef _FOUNDATION_IMAGE_H
#define _FOUNDATION_IMAGE_H

#include <stdint.h>

#include <swingby/size.h>

#ifdef __cplusplus
extern "C" {
#endif

enum sb_image_format {
    SB_IMAGE_FORMAT_RGBA32,
    SB_IMAGE_FORMAT_ARGB32,
};

typedef struct sb_image_t sb_image_t;

sb_image_t* sb_image_new(const sb_size_i_t *size, enum sb_image_format format);

const sb_size_i_t* sb_image_size(sb_image_t *image);

enum sb_image_format sb_image_format(sb_image_t *image);

uint8_t* sb_image_data(sb_image_t *image);

void sb_image_free(sb_image_t *image);

#ifdef __cplusplus
}
#endif

#endif /* _FOUNDATION_IMAGE_H */
