#include "draw.h"

#include <GL/gl.h>

#include "skia/include/core/SkRect.h"
#include "skia/include/core/SkRRect.h"
#include "skia/include/core/SkColor.h"
#include "skia/include/core/SkBitmap.h"
#include "skia/include/core/SkImage.h"
#include "skia/include/core/SkCanvas.h"
#include "skia/include/core/SkFont.h"
#include "skia/include/core/SkTextBlob.h"

#include "skia/include/gpu/ganesh/GrDirectContext.h"
// GrBackendRenderTargets::MakeGL
#include "skia/include/gpu/ganesh/gl/GrGLBackendSurface.h"
// SkImages::BorrowTextureFrom
#include "skia/include/gpu/ganesh/SkImageGanesh.h"

#include "skia/include/effects/SkImageFilters.h"

#include "./renderer.h"
#include "./raster-renderer.h"
#include "./gl-renderer.h"

#include <swingby/view.h>
#include <swingby/list.h>
#include <swingby/filter.h>
#include <swingby/glyph.h>
#include <swingby/log.h>
#include <swingby/bench.h>
// For draw glyphs. Temporary include.
#include <swingby/canvas.h>

#include "../helpers/shared.h"
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
        rect->position.x, rect->position.y,
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
        rect->position.x, rect->position.y,
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
        rect->position.x * scale,
        rect->position.y * scale,
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

void sb_skia_draw_rect3(sb_skia_renderer_t *renderer,
                        const sb_view_t *view,
                        float scale)
{
    SkCanvas *canvas = _get_canvas(renderer);

    // View's properties.
    sb_rect_t rect = sb_view_geometry(view);
    sb_color_t color = sb_view_color(view);
    const sb_view_radius_t *radius = sb_view_radius(view);
    const sb_list_t *filters = sb_view_filters(view);

    enum sb_rounding_policy p_policy = sb_view_position_rounding_policy(view);
    enum sb_rounding_policy s_policy = sb_view_size_rounding_policy(view);

    float (*pos_rounding_f)(float) = _rounding_function(p_policy);
    float (*size_rounding_f)(float) = _rounding_function(s_policy);

    SkRect sk_rect = SkRect::MakeXYWH(
        pos_rounding_f(rect.position.x * scale),
        pos_rounding_f(rect.position.y * scale),
        size_rounding_f(rect.size.width * scale),
        size_rounding_f(rect.size.height * scale));

    SkPaint paint;
    SkColor4f sk_color_4f;
    sk_color_4f.fR = color.r;
    sk_color_4f.fG = color.g;
    sk_color_4f.fB = color.b;
    sk_color_4f.fA = color.a;
    paint.setColor(sk_color_4f);
    if (sb_view_antialiased(view)) {
        paint.setAntiAlias(true);
    }

    // For restore stack.
    int save_count = 0;

    if (sb_list_length(filters) > 0) {
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

    sb_size_i_t image_size = sb_image_size((sb_image_t*)image);

    SkImageInfo image_info = SkImageInfo::Make(
        image_size.width,
        image_size.height,
        kRGBA_8888_SkColorType,
        kUnpremul_SkAlphaType
    );

    SkBitmap bitmap;
    bitmap.installPixels(image_info,
        sb_image_data((sb_image_t*)image),
        image_size.width * 4);

    sk_sp<SkImage> sk_image = SkImages::RasterFromBitmap(bitmap);

    SkRect sk_rect = SkRect::MakeXYWH(
        rect->position.x * scale,
        rect->position.y * scale,
        rect->size.width * scale,
        rect->size.height * scale);

    SkSamplingOptions sampling;

    canvas->drawImageRect(sk_image, sk_rect, sampling, nullptr);
}

void sb_skia_draw_image3(sb_skia_renderer_t *renderer,
                         const sb_view_t *view,
                         float scale)
{
    SkCanvas *canvas = _get_canvas(renderer);
    sb_image_t *image = sb_view_image((sb_view_t*)view);
    if (image == NULL) {
        sb_log_warn("Image is NULL!\n");
        return;
    }

    sb_rect_t rect = sb_view_geometry(view);
    sb_size_i_t image_size = sb_image_size(image);

    // Make texture.
    auto gl_renderer =
        (sb_skia_gl_renderer_t*)sb_skia_renderer_current(renderer);
    (void)gl_renderer;
    // sb_skia_gl_renderer_make_image_texture(gl_renderer, impl);

    // TODO: RRect.

    SkRect sk_rect = SkRect::MakeXYWH(
        rect.position.x * scale,
        rect.position.y * scale,
        rect.size.width * scale,
        rect.size.height * scale
    );

    SkSamplingOptions sampling;
    if (sb_view_antialiased(view)) {
        if (rect.size.width > image_size.width &&
            rect.size.height > image_size.height) {
            // Upscale.
            sampling = SkSamplingOptions(
                SkFilterMode::kLinear,
                SkMipmapMode::kLinear
            );
        } else {
            // Downscale.
            sampling = SkSamplingOptions(
                SkFilterMode::kLinear,
                SkMipmapMode::kLinear
            );
            //sampling = SkSamplingOptions(SkCubicResampler::Mitchell());
        }
    }

    sk_sp<SkImage> sk_image = *(sk_sp<SkImage>*)sb_image_sk_image(image);
    canvas->drawImageRect(sk_image,
        sk_rect, sampling, nullptr);
}

void sb_skia_draw_glyphs(sb_skia_renderer_t *renderer,
                         const sb_rect_t *rect,
                         float scale,
                         const sb_glyph_block_t *block)
{
    // SkCanvas *canvas = _get_canvas(renderer);
    sb_canvas_t *canvas = sb_canvas_new(_get_canvas(renderer));
    sb_canvas_set_scale(canvas, scale);
    sb_canvas_set_position(canvas, &rect->position);

    sb_point_t zero = { .x = 0.0f, .y = 0.0f };
    sb_canvas_draw_glyphs(canvas, block, zero);

    sb_canvas_free(canvas);
}

void sb_skia_draw_texture(sb_skia_renderer_t *renderer,
                          const sb_rect_t *rect,
                          float scale,
                          uint32_t texture_id)
{
    SkCanvas *canvas = _get_canvas(renderer);
    auto gl_renderer =
        (sb_skia_gl_renderer_t*)sb_skia_renderer_current(renderer);

    GrGLTextureInfo info;
    info.fTarget = GL_TEXTURE_2D;
    info.fID = texture_id;
    info.fFormat = GL_RGBA8;

    GrBackendTexture backend_texture = GrBackendTextures::MakeGL(
        rect->size.width, rect->size.height,
        skgpu::Mipmapped::kNo,
        info
    );

    sk_sp<SkImage> image = SkImages::BorrowTextureFrom(
        static_cast<GrDirectContext*>(
            sb_skia_gl_renderer_direct_context(gl_renderer)
        ),
        backend_texture,
        kBottomLeft_GrSurfaceOrigin,
        kRGBA_8888_SkColorType,
        kPremul_SkAlphaType,
        nullptr
    );

    SkRect sk_rect = SkRect::MakeXYWH(
        rect->position.x * scale,
        rect->position.y * scale,
        rect->size.width * scale,
        rect->size.height * scale
    );
    // TODO: What below option for? Add detailed comment.
    SkSamplingOptions sampling(SkFilterMode::kLinear);

    canvas->drawImageRect(image, sk_rect, sampling);
}

void sb_skia_clip_rect(sb_skia_renderer_t *renderer,
                       const sb_rect_t *rect,
                       const sb_view_radius_t *radius,
                       float scale)
{
    SkCanvas *canvas = _get_canvas(renderer);

    SkRect sk_rect = SkRect::MakeXYWH(
        rect->position.x * scale,
        rect->position.y * scale,
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
