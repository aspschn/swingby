#ifndef _SWINGBY_SKIA_LOAD_IMAGE_H
#define _SWINGBY_SKIA_LOAD_IMAGE_H

#include <stdbool.h>
#include <stdint.h>

#include <swingby/image.h>

#ifdef __cplusplus
extern "C" {
#endif

uint8_t* sb_skia_load_image_from_file(const char *filename,
                                      enum sb_image_file_format format,
                                      uint64_t *width,
                                      uint64_t *height);

void sb_skia_image_draw_image(sb_image_t *dst,
                              const sb_image_t *src,
                              const sb_point_i_t *pos,
                              enum sb_blend_mode blend_mode);

void sb_skia_image_fill(sb_image_t *image, const sb_color_t *color);

#ifdef __cplusplus
}
#endif

#endif /* _SWINGBY_SKIA_LOAD_IMAGE_H */
