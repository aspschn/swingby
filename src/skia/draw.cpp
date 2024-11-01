#include "draw.h"

#include "skia/include/core/SkRect.h"
#include "skia/include/core/SkRRect.h"
#include "skia/include/core/SkColor.h"
#include "skia/include/core/SkBitmap.h"
#include "skia/include/core/SkImage.h"

#include "./gl-context.h"
#include "./raster-context.h"

#include <swingby/view.h>

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

void sb_skia_clear(sb_skia_context_t *context,
                   const sb_color_t *color)
{
    SkCanvas *canvas = _get_canvas(context);

    canvas->clear(SkColorSetARGB(color->a, color->r, color->g, color->b));
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

void sb_skia_draw_rect_with_radius(sb_skia_context_t *context,
                                   const sb_rect_t *rect,
                                   const sb_color_t *color,
                                   const sb_view_radius_t *radius)
{
    SkCanvas *canvas = _get_canvas(context);

    SkRect sk_rect = SkRect::MakeXYWH(
        rect->pos.x, rect->pos.y,
        rect->size.width, rect->size.height);

    float top_left = sb_view_radius_top_left(radius);
    float top_right = sb_view_radius_top_right(radius);
    float bottom_right = sb_view_radius_bottom_right(radius);
    float bottom_left = sb_view_radius_bottom_left(radius);
    SkVector radii[] = {
        { top_left, top_left },
        { top_right, top_right },
        { bottom_right, bottom_right },
        { bottom_left, bottom_left },
    };

    SkRRect rrect;
    rrect.setRectRadii(sk_rect, radii);

    SkPaint paint;
    paint.setColor(SkColorSetARGB(color->a, color->r, color->g, color->b));
    canvas->drawRRect(rrect, paint);
}

void sb_skia_draw_image(sb_skia_context_t *context,
                        const sb_rect_t *rect,
                        const sb_image_t *image)
{
    SkCanvas *canvas = _get_canvas(context);

    const sb_size_i_t *image_size = sb_image_size((sb_image_t*)image);

    SkImageInfo image_info = SkImageInfo::Make(
        image_size->width,
        image_size->height,
        kRGBA_8888_SkColorType,
        kPremul_SkAlphaType
    );

    SkBitmap bitmap;
    bitmap.installPixels(image_info,
        sb_image_data((sb_image_t*)image),
        image_size->width * 4);

    sk_sp<SkImage> sk_image = SkImages::RasterFromBitmap(bitmap);

    SkRect sk_rect = SkRect::MakeXYWH(
        rect->pos.x, rect->pos.y,
        rect->size.width, rect->size.height);

    SkSamplingOptions sampling;

    canvas->drawImageRect(sk_image, sk_rect, sampling, nullptr);
}

void sb_skia_save_pos(sb_skia_context_t *context, const sb_point_t *pos)
{
    SkCanvas *canvas = _get_canvas(context);

    canvas->save();
    canvas->translate(pos->x, pos->y);
}

void sb_skia_restore_pos(sb_skia_context_t *context)
{
    SkCanvas *canvas = _get_canvas(context);

    canvas->restore();
}

#ifdef __cplusplus
}
#endif
