#ifndef _SWINGBY_SKIA_IMAGE_H
#define _SWINGBY_SKIA_IMAGE_H

#include <stdint.h>

#include <swingby/size.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef struct sb_skia_image_t sb_skia_image_t;

sb_skia_image_t* sb_skia_image_new();

bool sb_skia_image_is_null(const sb_skia_image_t* image);

bool sb_skia_image_load_from_data(sb_skia_image_t *image,
                                  const uint8_t *data,
                                  uint64_t len);

const sb_size_i_t* sb_skia_image_size(const sb_skia_image_t *image);

void* sb_skia_image_sk_bitmap(const sb_skia_image_t *image);

void sb_skia_image_free(sb_skia_image_t *image);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* _SWINGBY_SKIA_IMAGE_H */
