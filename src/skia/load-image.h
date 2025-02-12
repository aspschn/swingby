#ifndef _SWINGBY_SKIA_LOAD_IMAGE_H
#define _SWINGBY_SKIA_LOAD_IMAGE_H

#include <stdbool.h>

#include <swingby/image.h>

#ifdef __cplusplus
extern "C" {
#endif

uint8_t* sb_skia_load_image_from_file(const char *filename,
                                      enum sb_image_file_format format,
                                      uint64_t *width,
                                      uint64_t *height);

#ifdef __cplusplus
}
#endif

#endif /* _SWINGBY_SKIA_LOAD_IMAGE_H */
