#include <swingby/rect.h>

#ifdef __cplusplus
extern "C" {
#endif

bool sb_rect_contains_point(sb_rect_t *rect, const sb_point_t *point)
{
    // Check top-left.
    if (point->x < rect->position.x || point->y < rect->position.y) {
        return false;
    }

    // Check bottom-right.
    if (point->x > rect->position.x + rect->size.width ||
        point->y > rect->position.y + rect->size.height) {
        return false;
    }

    // Check width and height.
    if (point->x <= rect->position.x + rect->size.width &&
        point->y <= rect->position.y + rect->size.height) {
        return true;
    }

    return false;
}

bool sb_rect_intersects(const sb_rect_t *rect, const sb_rect_t *other)
{
    if (rect->position.x + rect->size.width <= other->position.x) {
        return false;
    }
    if (rect->position.x >= other->position.x + other->size.width) {
        return false;
    }
    if (rect->position.y + rect->size.height <= other->position.y) {
        return false;
    }
    if (rect->position.y >= other->position.y + other->size.height) {
        return false;
    }

    return true;
}

bool sb_rect_equals(const sb_rect_t *rect, const sb_rect_t *other)
{
    if (rect->position.x == other->position.x &&
        rect->position.y == other->position.y &&
        rect->size.width == other->size.width &&
        rect->size.height == other->size.height) {
        return true;
    }

    return false;
}

#ifdef __cplusplus
}
#endif
