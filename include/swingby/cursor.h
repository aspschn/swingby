#ifndef _FOUNDATION_CURSOR_H
#define _FOUNDATION_CURSOR_H

#include <swingby/point.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct sb_surface_t sb_surface_t;

enum sb_cursor_shape {
    SB_CURSOR_SHAPE_NONE            = 0,
    /// Default arrow cursor.
    SB_CURSOR_SHAPE_DEFAULT         = 1,
    /// Context menu arrow. Menu icon next to the arrow.
    SB_CURSOR_SHAPE_CONTEXT_MENU    = 2,
    /// Question mark in a circle next to the arrow.
    SB_CURSOR_SHAPE_HELP            = 3,
    /// Finger pointing hand.
    SB_CURSOR_SHAPE_POINTER         = 4,
    /// Arrow cursor with animated waiting indicator.
    SB_CURSOR_SHAPE_PROGRESS        = 5,
    /// Animated waiting indicator.
    SB_CURSOR_SHAPE_WAIT            = 6,
    /// Thick cross.
    SB_CURSOR_SHAPE_CELL            = 7,
    /// Thin cross.
    SB_CURSOR_SHAPE_CROSSHAIR       = 8,
    /// I-Beam shape.
    SB_CURSOR_SHAPE_TEXT            = 9,
    /// Vertical I-Beam shape.
    SB_CURSOR_SHAPE_VERTICAL_TEXT   = 10,
    /// Curved arrow sign next to the arrow.
    SB_CURSOR_SHAPE_ALIAS           = 11,
    /// Copy arrow. Plus sign next to the arrow.
    SB_CURSOR_SHAPE_COPY            = 12,
    /// Top, bottom, left and right arrow or same as grabbing.
    SB_CURSOR_SHAPE_MOVE            = 13,
    /// Arrow with crossed circle.
    SB_CURSOR_SHAPE_NO_DROP         = 14,
    /// Red circle with diagonal in the circle.
    SB_CURSOR_SHAPE_NOT_ALLOWED     = 15,
    /// Open hand.
    SB_CURSOR_SHAPE_GRAB            = 16,
    /// Closed hand.
    SB_CURSOR_SHAPE_GRABBING        = 17,
    /// Resize arrow pointing right.
    SB_CURSOR_SHAPE_E_RESIZE        = 18,
    /// Risize arrow pointing top.
    SB_CURSOR_SHAPE_N_RESIZE        = 19,
    /// Resize arrow pointing top right.
    SB_CURSOR_SHAPE_NE_RESIZE       = 20,
    /// Resize arrow pointing top left.
    SB_CURSOR_SHAPE_NW_RESIZE       = 21,
    /// Resize arrow pointing bottom.
    SB_CURSOR_SHAPE_S_RESIZE        = 22,
    /// Resize arrow pointing bottom right.
    SB_CURSOR_SHAPE_SE_RESIZE       = 23,
    /// Resize arrow pointing bottom left,
    SB_CURSOR_SHAPE_SW_RESIZE       = 24,
    /// Resize arrow pointing left.
    SB_CURSOR_SHAPE_W_RESIZE        = 25,
    /// Resize arrow pointing both left and right.
    SB_CURSOR_SHAPE_EW_RESIZE       = 26,
    /// Resize arrow pointing both top and bottom.
    SB_CURSOR_SHAPE_NS_RESIZE       = 27,
    /// Resize arrow pointing both top right and bottom left.
    SB_CURSOR_SHAPE_NESW_RESIZE     = 28,
    /// Resize arrow pointing both top left and bottom right.
    SB_CURSOR_SHAPE_NWSE_RESIZE     = 29,
    /// Two arrows for left and right with a vertical bar.
    SB_CURSOR_SHAPE_COL_RESIZE      = 30,
    /// Tow arrows for top and bottom with a horizontal bar.
    SB_CURSOR_SHAPE_ROW_RESIZE      = 31,
    /// Four arrows for top, left, right and bottom with a dot in center.
    SB_CURSOR_SHAPE_ALL_SCROLL      = 32,
    /// Magnifier with plus sign.
    SB_CURSOR_SHAPE_ZOOM_IN         = 33,
    /// Magnifier with minus sign.
    SB_CURSOR_SHAPE_ZOOM_OUT        = 34,

    /// Cross cursor.
    SB_CURSOR_SHAPE_CROSS,
    /// Red circle with diagonal in the circle.
    SB_CURSOR_SHAPE_CROSSED_CIRCLE,
    /// Arrow with CrossedCircle.
    SB_CURSOR_SHAPE_FORBIDDEN,
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
