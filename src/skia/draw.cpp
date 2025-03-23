#include "draw.h"

#include "skia/include/core/SkRect.h"
#include "skia/include/core/SkRRect.h"
#include "skia/include/core/SkColor.h"
#include "skia/include/core/SkBitmap.h"
#include "skia/include/core/SkImage.h"
#include "skia/include/effects/SkImageFilters.h"

#include "./gl-context.h"
#include "./raster-context.h"

#include <swingby/view.h>
#include <swingby/list.h>
#include <swingby/filter.h>

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

void sb_skia_draw_rect2(sb_skia_context_t *context,
                        const sb_rect_t *rect,
                        uint32_t scale,
                        const sb_color_t *color,
                        const sb_view_radius_t *radius,
                        const sb_list_t *filters,
                        bool clip)
{
    SkCanvas *canvas = _get_canvas(context);

    SkRect sk_rect = SkRect::MakeXYWH(
        rect->pos.x * scale,
        rect->pos.y * scale,
        rect->size.width * scale,
        rect->size.height * scale);

    SkPaint paint;
    paint.setColor(SkColorSetARGB(color->a, color->r, color->g, color->b));

    // For restore stack.
    int save_count = 0;

    if (filters != NULL) {
        SkPaint filter_paint;
        sk_sp<SkImageFilter> prev_filter = nullptr; // For multiple filters,
                                                    // store the previous one.
        for (int i = 0; i < sb_list_length((sb_list_t*)filters); ++i) {
            const sb_filter_t *filter = (sb_filter_t*)sb_list_at((sb_list_t*)filters, i);
            if (sb_filter_type(filter) == SB_FILTER_TYPE_BLUR) {
                float radius = sb_filter_blur_radius(filter);

                sk_sp<SkImageFilter> blur_filter = SkImageFilters::Blur(
                    radius, radius, SkTileMode::kClamp, prev_filter, {});
                filter_paint.setImageFilter(blur_filter);

                prev_filter = blur_filter;
            } else if (sb_filter_type(filter) == SB_FILTER_TYPE_DROP_SHADOW) {
                const sb_point_t *offset = sb_filter_drop_shadow_offset(filter);
                float radius = sb_filter_drop_shadow_radius(filter);
                const sb_color_t *color = sb_filter_drop_shadow_color(filter);

                auto sk_color = SkColorSetARGB(
                    color->a, color->r, color->g, color->b);
                sk_sp<SkImageFilter> shadow_filter = SkImageFilters::DropShadow(
                    offset->x, offset->y,
                    radius, radius,
                    sk_color,
                    prev_filter,
                    {}
                );
                filter_paint.setImageFilter(shadow_filter);

                prev_filter = shadow_filter;
            }
        }
        canvas->saveLayer(nullptr, &filter_paint);
        ++save_count;
    }

    if (radius != NULL) {
        float top_left = sb_view_radius_top_left(radius) * scale;
        float top_right = sb_view_radius_top_right(radius) * scale;
        float bottom_right = sb_view_radius_bottom_right(radius) * scale;
        float bottom_left = sb_view_radius_bottom_left(radius) * scale;
        SkVector radii[] = {
            { top_left, top_left },
            { top_right, top_right },
            { bottom_right, bottom_right },
            { bottom_left, bottom_left },
        };

        SkRRect rrect;
        rrect.setRectRadii(sk_rect, radii);

        // Clip rrect.
        if (clip == true) {
            canvas->clipRRect(rrect);
        }

        canvas->drawRRect(rrect, paint);
        for (int i = 0; i < save_count; ++i) {
            canvas->restore();
        }
        return;
    }

    // Clip normal rect.
    if (clip == true) {
        canvas->clipRect(sk_rect);
    }
    canvas->drawRect(sk_rect, paint);
    for (int i = 0; i < save_count; ++i) {
        canvas->restore();
    }

    // Restore clip.
    if (clip == true) {
        canvas->restore();
    }
}

void sb_skia_draw_image(sb_skia_context_t *context,
                        const sb_rect_t *rect,
                        uint32_t scale,
                        const sb_image_t *image)
{
    SkCanvas *canvas = _get_canvas(context);

    const sb_size_i_t *image_size = sb_image_size((sb_image_t*)image);

    SkImageInfo image_info = SkImageInfo::Make(
        image_size->width,
        image_size->height,
        kRGBA_8888_SkColorType,
        kUnpremul_SkAlphaType
    );

    SkBitmap bitmap;
    bitmap.installPixels(image_info,
        sb_image_data((sb_image_t*)image),
        image_size->width * 4);

    sk_sp<SkImage> sk_image = SkImages::RasterFromBitmap(bitmap);

    SkRect sk_rect = SkRect::MakeXYWH(
        rect->pos.x * scale,
        rect->pos.y * scale,
        rect->size.width * scale,
        rect->size.height * scale);

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
