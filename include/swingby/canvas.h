#ifndef _SWINGBY_CANVAS_H
#define _SWINGBY_CANVAS_H

#include <swingby/common.h>
#include <swingby/rect.h>
#include <swingby/paint.h>

typedef struct sb_canvas_t sb_canvas_t;

#ifdef __cplusplus
extern "C" {
#endif

/// \brief Create a new canvas.
SB_EXPORT
sb_canvas_t* sb_canvas_new(void *sk_canvas);

/// \brief Get the default paint of the canvas.
SB_EXPORT
sb_paint_t* sb_canvas_paint(sb_canvas_t *canvas);

SB_EXPORT
void sb_canvas_draw_rect(sb_canvas_t *canvas,
                         const sb_rect_t *rect,
                         const sb_paint_t *paint);

SB_EXPORT
void sb_canvas_free(sb_canvas_t *canvas);

#ifdef __cplusplus
}
#endif

#endif /* _SWINGBY_CANVAS_H */
