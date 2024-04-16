#include <swingby/rect.h>

#ifdef __cplusplus
extern "C" {
#endif

bool ft_rect_contains_point(ft_rect_t *rect, const ft_point_t *point)
{
    // Check top-left.
    if (point->x < rect->pos.x || point->y < rect->pos.y) {
        return false;
    }

    // Check bottom-right.
    if (point->x > rect->pos.x + rect->size.width ||
        point->y > rect->pos.y + rect->size.height) {
        return false;
    }

    // Check width and height.
    if (point->x <= rect->pos.x + rect->size.width &&
        point->y <= rect->pos.y + rect->size.height) {
        return true;
    }

    return false;
}

#ifdef __cplusplus
}
#endif
