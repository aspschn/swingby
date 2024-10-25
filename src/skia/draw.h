#ifndef _SWINGBY_SKIA_DRAW_H
#define _SWINGBY_SKIA_DRAW_H

#include <swingby/rect.h>
#include <swingby/color.h>

#include "./context.h"

#ifdef __cplusplus
extern "C" {
#endif

void sb_skia_clear(sb_skia_context_t *context,
                   const sb_color_t *color);

void sb_skia_draw_rect(sb_skia_context_t *context,
                       const sb_rect_t *rect,
                       const sb_color_t *color);

void sb_skia_save_pos(sb_skia_context_t *context,
                      const sb_point_t *pos);

void sb_skia_restore_pos(sb_skia_context_t *context);

#ifdef __cplusplus
}
#endif

#endif /* _SWINGBY_SKIA_DRAW_H */
