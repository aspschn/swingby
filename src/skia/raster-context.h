#ifndef _SWINGBY_SKIA_RASTER_CONTEXT_H
#define _SWINGBY_SKIA_RASTER_CONTEXT_H

#include <stdint.h>

#include <memory>
#include <vector>

#include "skia/include/core/SkCanvas.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct sb_skia_raster_context_t {
    std::unique_ptr<SkCanvas> canvas;
    std::vector<uint32_t> buffer;
} sb_skia_raster_context_t;

sb_skia_raster_context_t* sb_skia_raster_context_new();

void sb_skia_raster_context_free(sb_skia_raster_context_t *raster_context);

#ifdef __cplusplus
}
#endif

#endif /* _SWINGBY_SKIA_RASTER_CONTEXT_H */
