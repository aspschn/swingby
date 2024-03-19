#ifndef _FOUNDATION_CURSOR_H
#define _FOUNDATION_CURSOR_H

#include <foundation/point.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ft_surface_t ft_surface_t;

typedef enum ft_cursor_shape {
    FT_CURSOR_SHAPE_NONE,
    /// Default arrow cursor.
    FT_CURSOR_SHAPE_ARROW,
    /// Copy arrow. Plus sign next to the arrow.
    FT_CURSOR_SHAPE_COPY,
    /// Context menu arrow. Menu icon next to the arrow.
    FT_CURSOR_SHAPE_CONTEXT_MENU,
    /// Cross cursor.
    FT_CURSOR_SHAPE_CROSS,
    /// Red circle with diagonal in the circle.
    FT_CURSOR_SHAPE_CROSSED_CIRCLE,
    /// Arrow with CrossedCircle.
    FT_CURSOR_SHAPE_FORBIDDEN,
    /// Question mark in a circle next to the arrow.
    FT_CURSOR_SHAPE_HELP,
    /// Curved arrow in a circle next to the arrow.
    FT_CURSOR_SHAPE_LINK,
    /// Four arrows for top, left, right and bottom with a dot in center.
    FT_CURSOR_SHAPE_SIZE_ALL,
    /// Two arrows for top-right and bottom-left with a dot in center.
    FT_CURSOR_SHAPE_SIZE_BDIAG,
    /// Two arrows for top-left and bottom-right with a dot in center.
    FT_CURSOR_SHAPE_SIZE_FDIAG,
    /// Two arrows for left and right with a dot in center.
    /// Usually used in resize window horizontally.
    FT_CURSOR_SHAPE_SIZE_HOR,
    /// Two arrows for top and bottom with a dot in center.
    /// Usually used in resize window vertically.
    FT_CURSOR_SHAPE_SIZE_VER,
    /// Two arrows for left and right with a vertical bar.
    FT_CURSOR_SHAPE_SPLIT_HOR,
    /// Tow arrows for top and bottom with a horizontal bar.
    FT_CUSOR_SHAPE_SPLIT_VER,
} ft_cursor_shape;

typedef struct ft_cursor_t ft_cursor_t;

ft_cursor_t* ft_cursor_new(ft_cursor_shape shape, const ft_point_t *hot_spot);

ft_surface_t* ft_cursor_surface(ft_cursor_t *cursor);

#ifdef __cplusplus
}
#endif

#endif /* _FOUNDATION_CURSOR_H */
