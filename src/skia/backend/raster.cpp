#include <stdint.h>

#include "skia/include/core/SkCanvas.h"

#include "../raster-context.h"

#ifdef __cplusplus
extern "C" {
#endif

void sb_skia_raster_begin(sb_skia_raster_context_t *context,
                          uint32_t width,
                          uint32_t height)
{
    context->canvas = SkCanvas::MakeRasterDirectN32(width, height,
        context->buffer.data(), sizeof(uint32_t) * width);
}

void sb_skia_raster_end(sb_skia_raster_context_t *context)
{
    context->canvas.release();
}

#ifdef __cplusplus
}
#endif
