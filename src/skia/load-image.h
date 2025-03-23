#ifndef _SWINGBY_SKIA_LOAD_IMAGE_H
#define _SWINGBY_SKIA_LOAD_IMAGE_H

#include <stdbool.h>
#include <stdint.h>

#include <swingby/image.h>

#ifdef __cplusplus
extern "C" {
#endif

/// \brief Load image from file or data.
///
/// It is changed to function that load image file or data.
/// IT WAS only for loading image from file.
/// Because i'm lazy, not changed it's function name.
/// If `filename` NULL, try read from `data` with `data_len`.
uint8_t* sb_skia_load_image_from_file(const char *filename,
                                      const uint8_t *data,
                                      uint64_t data_len,
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
