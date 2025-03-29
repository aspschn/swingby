#ifndef _FOUNDATION_IMAGE_H
#define _FOUNDATION_IMAGE_H

#include <stdbool.h>
#include <stdint.h>

#include <swingby/common.h>
#include <swingby/point.h>
#include <swingby/size.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct sb_color_t sb_color_t;

enum sb_image_format {
    SB_IMAGE_FORMAT_RGBA32  = 0,
    SB_IMAGE_FORMAT_ARGB32  = 1,
};

enum sb_image_file_format {
    SB_IMAGE_FILE_FORMAT_PNG    = 1,
    SB_IMAGE_FILE_FORMAT_JPEG   = 2,
    SB_IMAGE_FILE_FORMAT_AUTO   = 255,
};

enum sb_blend_mode {
    SB_BLEND_MODE_NONE              = 0,
    SB_BLEND_MODE_ALPHA             = 1,
    SB_BLEND_MODE_PREMULTIPLIED     = 2,
};

typedef struct sb_image_t sb_image_t;

SB_EXPORT
sb_image_t* sb_image_new(const sb_size_i_t *size, enum sb_image_format format);

SB_EXPORT
const sb_size_i_t* sb_image_size(const sb_image_t *image);

SB_EXPORT
enum sb_image_format sb_image_format(const sb_image_t *image);

SB_EXPORT
uint8_t* sb_image_data(sb_image_t *image);

SB_EXPORT
void sb_image_set_data(sb_image_t *image,
                       const uint8_t *data,
                       const sb_size_i_t *size);

SB_EXPORT
void sb_image_fill(sb_image_t *image, const sb_color_t *color);

SB_EXPORT
void sb_image_draw_image(sb_image_t *image,
                         const sb_image_t *src,
                         const sb_point_i_t *pos,
                         enum sb_blend_mode blend_mode);

SB_EXPORT
bool sb_image_load_from_file(sb_image_t *image,
                             const char *filename,
                             enum sb_image_file_format format);

SB_EXPORT
bool sb_image_load_from_data(sb_image_t *image,
                             const uint8_t *data,
                             uint64_t data_len,
                             enum sb_image_file_format format);

SB_EXPORT
void sb_image_free(sb_image_t *image);

#ifdef __cplusplus
}
#endif

#endif /* _FOUNDATION_IMAGE_H */
