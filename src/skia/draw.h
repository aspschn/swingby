#ifndef _SWINGBY_SKIA_DRAW_H
#define _SWINGBY_SKIA_DRAW_H

#include <swingby/rect.h>
#include <swingby/color.h>
#include <swingby/image.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct sb_skia_renderer_t sb_skia_renderer_t;

typedef struct sb_view_radius_t sb_view_radius_t;
typedef struct sb_list_t sb_list_t;

void sb_skia_clear(sb_skia_renderer_t *renderer,
                   const sb_color_t *color);

void sb_skia_draw_rect(sb_skia_renderer_t *renderer,
                       const sb_rect_t *rect,
                       const sb_color_t *color);

/*
void sb_skia_draw_rect_with_radius(sb_skia_context_t *context,
                                   const sb_rect_t *rect,
                                   const sb_color_t *color,
                                   const sb_view_radius_t *radius);
*/

/// \brief Draw a rectangle with the given properties.
///
/// \param renderer The renderer.
/// \param rect     The Geometry of this rectangle.
/// \param color    The color of this rectangle.
/// \param radius   The radius of this rectangle. Can be NULL.
/// \param filters  The filter list of this rectangle. Can be NULL.
void sb_skia_draw_rect2(sb_skia_renderer_t *renderer,
                        const sb_rect_t *rect,
                        uint32_t scale,
                        const sb_color_t *color,
                        const sb_view_radius_t *radius,
                        const sb_list_t *filters,
                        bool clip);

void sb_skia_draw_image(sb_skia_renderer_t *renderer,
                        const sb_rect_t *rect,
                        uint32_t scale,
                        const sb_image_t *image);

void sb_skia_draw_image2(sb_skia_renderer_t *renderer,
                         const sb_rect_t *rect,
                         uint32_t scale,
                         const sb_image_t *image);

void sb_skia_clip_rect(sb_skia_renderer_t *renderer,
                       const sb_rect_t *rect,
                       const sb_view_radius_t *radius,
                       uint32_t scale);

void sb_skia_clip_restore(sb_skia_renderer_t *renderer);

void sb_skia_save_pos(sb_skia_renderer_t *renderer,
                      const sb_point_t *pos);

void sb_skia_restore_pos(sb_skia_renderer_t *renderer);

#ifdef __cplusplus
}
#endif

#endif /* _SWINGBY_SKIA_DRAW_H */
