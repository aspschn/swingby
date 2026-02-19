#include "draw.h"

#include "skia/include/core/SkRect.h"
#include "skia/include/core/SkRRect.h"
#include "skia/include/core/SkColor.h"
#include "skia/include/core/SkBitmap.h"
#include "skia/include/core/SkImage.h"
#include "skia/include/core/SkCanvas.h"
#include "skia/include/core/SkFontMgr.h"
#include "skia/include/core/SkTypeface.h"
#include "skia/include/core/SkFont.h"
#include "skia/include/core/SkTextBlob.h"
#include "skia/include/effects/SkImageFilters.h"
#include "skia/include/ports/SkFontMgr_directory.h"

#include "./renderer.h"
#include "./raster-renderer.h"
#include "./gl-renderer.h"

#include <swingby/view.h>
#include <swingby/list.h>
#include <swingby/filter.h>
#include <swingby/glyph.h>
#include <swingby/log.h>

#include "../impl/image-impl.hpp"

#ifdef __cplusplus
extern "C" {
#endif

static SkCanvas* _get_canvas(sb_skia_renderer_t *renderer)
{
    SkCanvas *canvas = nullptr;
    enum sb_skia_backend backend = sb_skia_renderer_backend(renderer);
    void *ptr = sb_skia_renderer_current(renderer);
    if (backend == SB_SKIA_BACKEND_GL) {
        sb_skia_gl_renderer_t *renderer = (sb_skia_gl_renderer_t*)ptr;
        canvas = (SkCanvas*)sb_skia_gl_renderer_canvas(renderer);
    } else if (backend == SB_SKIA_BACKEND_RASTER) {
        sb_skia_raster_renderer_t *renderer = (sb_skia_raster_renderer_t*)ptr;
        canvas = (SkCanvas*)sb_skia_raster_renderer_canvas(renderer);
    }

    return canvas;
}

void sb_skia_clear(sb_skia_renderer_t *renderer,
                   const sb_color_t *color)
{
    SkCanvas *canvas = _get_canvas(renderer);

    canvas->clear(SkColorSetARGB(color->a, color->r, color->g, color->b));
}

void sb_skia_draw_rect(sb_skia_renderer_t *renderer,
                       const sb_rect_t *rect,
                       const sb_color_t *color)
{
    SkCanvas *canvas = _get_canvas(renderer);

    SkRect sk_rect = SkRect::MakeXYWH(
        rect->pos.x, rect->pos.y,
        rect->size.width, rect->size.height);
    SkPaint paint;
    paint.setColor(SkColorSetARGB(color->a, color->r, color->g, color->b));
    canvas->drawRect(sk_rect, paint);
}

void sb_skia_draw_rect_with_radius(sb_skia_renderer_t *renderer,
                                   const sb_rect_t *rect,
                                   const sb_color_t *color,
                                   const sb_view_radius_t *radius)
{
    SkCanvas *canvas = _get_canvas(renderer);

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

void sb_skia_draw_rect2(sb_skia_renderer_t *renderer,
                        const sb_rect_t *rect,
                        uint32_t scale,
                        const sb_color_t *color,
                        const sb_view_radius_t *radius,
                        const sb_list_t *filters,
                        bool clip)
{
    SkCanvas *canvas = _get_canvas(renderer);

    SkRect sk_rect = SkRect::MakeXYWH(
        rect->pos.x * scale,
        rect->pos.y * scale,
        rect->size.width * scale,
        rect->size.height * scale);

    SkPaint paint;
    SkColor4f sk_color_4f;
    sk_color_4f.fR = color->r;
    sk_color_4f.fG = color->g;
    sk_color_4f.fB = color->b;
    sk_color_4f.fA = color->a;
    paint.setColor(sk_color_4f);

    // For restore stack.
    int save_count = 0;

    if (filters != NULL) {
        SkPaint filter_paint;
        sk_sp<SkImageFilter> prev_filter = nullptr; // For multiple filters,
                                                    // store the previous one.
        for (uint64_t i = 0; i < sb_list_length((sb_list_t*)filters); ++i) {
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

                SkColor4f sk_color;
                sk_color.fR = color->r;
                sk_color.fG = color->g;
                sk_color.fB = color->b;
                sk_color.fA = color->a;
                sk_sp<SkImageFilter> shadow_filter = SkImageFilters::DropShadow(
                    offset->x, offset->y,
                    radius, radius,
                    sk_color.toSkColor(),
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

        canvas->drawRRect(rrect, paint);
        for (int i = 0; i < save_count; ++i) {
            canvas->restore();
        }
        return;
    }

    canvas->drawRect(sk_rect, paint);
    for (int i = 0; i < save_count; ++i) {
        canvas->restore();
    }
}

void sb_skia_draw_image(sb_skia_renderer_t *renderer,
                        const sb_rect_t *rect,
                        uint32_t scale,
                        const sb_image_t *image)
{
    SkCanvas *canvas = _get_canvas(renderer);

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

void sb_skia_draw_image2(sb_skia_renderer_t *renderer,
                         const sb_rect_t *rect,
                         uint32_t scale,
                         const sb_image_t *image)
{
    SkCanvas *canvas = _get_canvas(renderer);
    SbImageImpl *impl = sb_image_impl(image);

    // Make texture.
    auto gl_renderer =
        (sb_skia_gl_renderer_t*)sb_skia_renderer_current(renderer);

    sb_skia_gl_renderer_make_image_texture(gl_renderer, impl);

    // TODO: RRect.

    SkRect sk_rect = SkRect::MakeXYWH(
        rect->pos.x * scale,
        rect->pos.y * scale,
        rect->size.width * scale,
        rect->size.height * scale
    );

    SkSamplingOptions sampling;

    canvas->drawImageRect(impl->sk_image(), sk_rect, sampling, nullptr);
}

void sb_skia_draw_glyphs(sb_skia_renderer_t *renderer,
                         const sb_rect_t *rect,
                         uint32_t scale,
                         const sb_glyph_layout_t *layout)
{
    SkCanvas *canvas = _get_canvas(renderer);

    sk_sp<SkFontMgr> font_manager = SkFontMgr_New_Custom_Directory("/usr/share/fonts/");
    SkTextBlobBuilder builder;

    float total_x = 0.0f;
    const sb_glyph_line_t **lines = sb_glyph_layout_lines(layout);
    auto line_count = sb_glyph_layout_line_count(layout);
    for (uint32_t i = 0; i < line_count; ++i) {
        const sb_glyph_run_t **runs = sb_glyph_line_runs(lines[i]);
        auto run_count = sb_glyph_line_run_count(lines[i]);
        for (uint32_t j = 0; j < run_count; ++j) {
            auto glyph_count = sb_glyph_run_count(runs[j]);
            auto glyphs = sb_glyph_run_glyphs((sb_glyph_run_t*)runs[j]);

            // Get font.
            const sb_font_t *font = sb_glyph_run_font(runs[j]);
            sk_sp<SkTypeface> typeface = font_manager->makeFromFile(
                font->path,
                font->ttc_index
            );
            // Null check.
            if (typeface == nullptr) {
                sb_log_warn(
                    "sb_skia_draw_glyphs - Invalid typeface: \"%s (%d)\".\n",
                    font->path, font->ttc_index
                );
                return;
            }
            SkFont sk_font = SkFont(typeface, font->size * scale);

            auto& run = builder.allocRunPos(sk_font, glyph_count);
            for (uint32_t i = 0; i < glyph_count; ++i) {
                run.glyphs[i] = glyphs[i].id;
                run.points()[i] = SkPoint::Make(total_x, glyphs[i].offset.y * scale);
                total_x += (glyphs[i].advance * scale) + (glyphs[i].offset.x * scale);
            }
        }
    }

    sk_sp<SkTextBlob> blob = builder.make();

    SkPaint paint;
    paint.setColor(SK_ColorBLACK);
    canvas->drawTextBlob(blob.get(), 0, 24, paint);
}

void sb_skia_clip_rect(sb_skia_renderer_t *renderer,
                       const sb_rect_t *rect,
                       const sb_view_radius_t *radius,
                       uint32_t scale)
{
    SkCanvas *canvas = _get_canvas(renderer);

    SkRect sk_rect = SkRect::MakeXYWH(
        rect->pos.x * scale,
        rect->pos.y * scale,
        rect->size.width * scale,
        rect->size.height * scale);

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
        canvas->save();
        canvas->clipRRect(rrect);

        return;
    }

    // Clip normal rect.
    canvas->save();
    canvas->clipRect(sk_rect);
}

void sb_skia_clip_restore(sb_skia_renderer_t *renderer)
{
    SkCanvas *canvas = _get_canvas(renderer);

    canvas->restore();
}

void sb_skia_save_pos(sb_skia_renderer_t *renderer, const sb_point_t *pos)
{
    SkCanvas *canvas = _get_canvas(renderer);

    canvas->save();
    canvas->translate(pos->x, pos->y);
}

void sb_skia_restore_pos(sb_skia_renderer_t *renderer)
{
    SkCanvas *canvas = _get_canvas(renderer);

    canvas->restore();
}

#ifdef __cplusplus
}
#endif
