#include <swingby/point.h>

#ifdef __cplusplus
extern "C" {
#endif

float sb_point_x(const sb_point_t *point)
{
    return point->x;
}

float sb_point_y(const sb_point_t *point)
{
    return point->y;
}

bool sb_point_equals(const sb_point_t *point, const sb_point_t *other)
{
    return point->x == other->x &&
           point->y == other->y;
}

#ifdef __cplusplus
}
#endif
