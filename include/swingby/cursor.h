#ifndef _FOUNDATION_CURSOR_H
#define _FOUNDATION_CURSOR_H

#include <swingby/point.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct sb_surface_t sb_surface_t;

enum sb_cursor_shape {
    SB_CURSOR_SHAPE_NONE,
    /// Default arrow cursor.
    SB_CURSOR_SHAPE_ARROW,
    /// Copy arrow. Plus sign next to the arrow.
    SB_CURSOR_SHAPE_COPY,
    /// Context menu arrow. Menu icon next to the arrow.
    SB_CURSOR_SHAPE_CONTEXT_MENU,
    /// Cross cursor.
    SB_CURSOR_SHAPE_CROSS,
    /// Red circle with diagonal in the circle.
    SB_CURSOR_SHAPE_CROSSED_CIRCLE,
    /// Arrow with CrossedCircle.
    SB_CURSOR_SHAPE_FORBIDDEN,
    /// Question mark in a circle next to the arrow.
    SB_CURSOR_SHAPE_HELP,
    /// Curved arrow in a circle next to the arrow.
    SB_CURSOR_SHAPE_LINK,
    /// Four arrows for top, left, right and bottom with a dot in center.
    SB_CURSOR_SHAPE_SIZE_ALL,
    /// Two arrows for top-right and bottom-left with a dot in center.
    SB_CURSOR_SHAPE_SIZE_BDIAG,
    /// Two arrows for top-left and bottom-right with a dot in center.
    SB_CURSOR_SHAPE_SIZE_FDIAG,
    /// Two arrows for left and right with a dot in center.
    /// Usually used in resize window horizontally.
    SB_CURSOR_SHAPE_SIZE_HOR,
    /// Two arrows for top and bottom with a dot in center.
    /// Usually used in resize window vertically.
    SB_CURSOR_SHAPE_SIZE_VER,
    /// Two arrows for left and right with a vertical bar.
    SB_CURSOR_SHAPE_SPLIT_HOR,
    /// Tow arrows for top and bottom with a horizontal bar.
    SB_CURSOR_SHAPE_SPLIT_VER,
};

typedef struct sb_cursor_t sb_cursor_t;

sb_cursor_t* sb_cursor_new(enum sb_cursor_shape shape,
                           const sb_point_t *hot_spot);

sb_surface_t* sb_cursor_surface(sb_cursor_t *cursor);

#ifdef __cplusplus
}
#endif

#endif /* _FOUNDATION_CURSOR_H */
