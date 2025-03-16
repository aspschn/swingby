#include <swingby/point.h>

#ifdef __cplusplus
extern "C" {
#endif

bool sb_point_equals(const sb_point_t *point, const sb_point_t *other)
{
    return point->x == other->x &&
           point->y == other->y;
}

#ifdef __cplusplus
}
#endif
