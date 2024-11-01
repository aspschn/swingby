#ifndef _SWINGBY_SKIA_DRAW_H
#define _SWINGBY_SKIA_DRAW_H

#include <swingby/rect.h>
#include <swingby/color.h>
#include <swingby/image.h>

#include "./context.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct sb_view_radius_t sb_view_radius_t;

void sb_skia_clear(sb_skia_context_t *context,
                   const sb_color_t *color);

void sb_skia_draw_rect(sb_skia_context_t *context,
                       const sb_rect_t *rect,
                       const sb_color_t *color);

void sb_skia_draw_rect_with_radius(sb_skia_context_t *context,
                                   const sb_rect_t *rect,
                                   const sb_color_t *color,
                                   const sb_view_radius_t *radius);

void sb_skia_draw_image(sb_skia_context_t *context,
                        const sb_rect_t *rect,
                        const sb_image_t *image);

void sb_skia_save_pos(sb_skia_context_t *context,
                      const sb_point_t *pos);

void sb_skia_restore_pos(sb_skia_context_t *context);

#ifdef __cplusplus
}
#endif

#endif /* _SWINGBY_SKIA_DRAW_H */
