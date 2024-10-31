#include <swingby/rect.h>

#ifdef __cplusplus
extern "C" {
#endif

bool sb_rect_contains_point(sb_rect_t *rect, const sb_point_t *point)
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

bool sb_rect_intersects(const sb_rect_t *rect, const sb_rect_t *other)
{
    if (rect->pos.x + rect->size.width <= other->pos.x) {
        return false;
    }
    if (rect->pos.x >= other->pos.x + other->size.width) {
        return false;
    }
    if (rect->pos.y + rect->size.height <= other->pos.y) {
        return false;
    }
    if (rect->pos.y >= other->pos.y + other->size.height) {
        return false;
    }

    return true;
}

#ifdef __cplusplus
}
#endif
