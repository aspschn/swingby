#ifndef _SWINGBY_BACKEND_RASTER_H
#define _SWINGBY_BACKEND_RASTER_H

#include "../raster-context.h"

#ifdef __cplusplus
extern "C" {
#endif

void sb_skia_raster_begin(sb_skia_raster_context_t *context,
                          uint32_t width,
                          uint32_t height);

void sb_skia_raster_end(sb_skia_raster_context_t *context);

#ifdef __cplusplus
}
#endif

#endif /* _SWINGBY_BACKEND_RASTER_H */
