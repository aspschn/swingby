#ifndef _SWINGBY_PIXMAP_H
#define _SWINGBY_PIXMAP_H

#include <stdint.h>

#include <swingby/size.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

enum sb_pixel_format {
    SB_PIXEL_FORMAT_RGBA32 = 0,
    SB_PIXEL_FORMAT_ARGB32 = 1,
};

typedef struct sb_pixmap_t sb_pixmap_t;

sb_pixmap_t* sb_pixmap_new(const uint8_t *pixels,
                           const sb_size_i_t *size,
                           uint64_t stride,
                           enum sb_pixel_format format);

const uint8_t* sb_pixmap_data(const sb_pixmap_t *pixmap);

uint64_t sb_pixmap_width(const sb_pixmap_t *pixmap);

uint64_t sb_pixmap_height(const sb_pixmap_t *pixmap);

void sb_pixmap_free(sb_pixmap_t *pixmap);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* _SWINGBY_PIXMAP_H */

