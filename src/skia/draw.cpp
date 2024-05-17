#include "draw.h"

#include "skia/include/core/SkRect.h"
#include "skia/include/core/SkColor.h"

#include "./gl-context.h"
#include "./raster-context.h"

#ifdef __cplusplus
extern "C" {
#endif

static SkCanvas* _get_canvas(sb_skia_context_t *context)
{
    SkCanvas *canvas = nullptr;
    enum sb_skia_backend backend = sb_skia_context_backend(context);
    if (backend == SB_SKIA_BACKEND_GL) {
        auto gl_context = static_cast<sb_skia_gl_context_t*>(sb_skia_context_gl_context(context));
        canvas = gl_context->surface->getCanvas();
    } else if (backend == SB_SKIA_BACKEND_RASTER) {
        auto raster_context = static_cast<sb_skia_raster_context_t*>(sb_skia_context_raster_context(context));
        canvas = raster_context->canvas.get();
    }

    return canvas;
}

void sb_skia_draw_rect(sb_skia_context_t *context,
                       const sb_rect_t *rect,
                       const sb_color_t *color)
{
    SkCanvas *canvas = _get_canvas(context);

    SkRect sk_rect = SkRect::MakeXYWH(
        rect->pos.x, rect->pos.y,
        rect->size.width, rect->size.height);
    SkPaint paint;
    paint.setColor(SkColorSetARGB(color->a, color->r, color->g, color->b));
    canvas->drawRect(sk_rect, paint);
}

#ifdef __cplusplus
}
#endif
