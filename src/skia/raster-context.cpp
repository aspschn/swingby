#include "raster-context.h"

sb_skia_raster_context_t* sb_skia_raster_context_new()
{
    sb_skia_raster_context_t *context = new sb_skia_raster_context_t;

    context->canvas = nullptr;

    return context;
}
