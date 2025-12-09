#include "raster-renderer.h"

#include <stdint.h>

#include <vector>
#include <memory>

#include "skia/include/core/SkCanvas.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

struct sb_skia_raster_renderer_t {
    std::unique_ptr<SkCanvas> canvas;
    std::vector<uint32_t> buffer;
};

sb_skia_raster_renderer_t* sb_skia_raster_renderer_new()
{
    auto *renderer = new sb_skia_raster_renderer_t;

    renderer->canvas = nullptr;

    return renderer;
}

void* sb_skia_raster_renderer_canvas(sb_skia_raster_renderer_t *renderer)
{
    return renderer->canvas.get();
}

void* sb_skia_raster_renderer_buffer(sb_skia_raster_renderer_t *renderer)
{
    return renderer->buffer.data();
}

void sb_skia_raster_renderer_begin(sb_skia_raster_renderer_t *renderer,
                                   int width,
                                   int height)
{
    // TODO: Check same size.
    renderer->buffer.resize(width * height);

    SkImageInfo imageInfo = SkImageInfo::Make(width, height,
    kRGBA_8888_SkColorType, kPremul_SkAlphaType);
    renderer->canvas = SkCanvas::MakeRasterDirect(imageInfo,
        renderer->buffer.data(), sizeof(uint32_t) * width);
}

void sb_skia_raster_renderer_end(sb_skia_raster_renderer_t *renderer)
{
    renderer->canvas.release();
}

void sb_skia_raster_renderer_free(sb_skia_raster_renderer_t *renderer)
{
    delete renderer;
}

#ifdef __cplusplus
}
#endif // __cplusplus
