#include <swingby/filter.h>

#include <stdlib.h>

#include <swingby/log.h>

#ifdef __cplusplus
extern "C" {
#endif

sb_filter_t* sb_filter_new(enum sb_filter_type type)
{
    sb_filter_t *filter = malloc(sizeof(sb_filter_t));

    filter->type = type;

    if (type == SB_FILTER_TYPE_BLUR) {
        filter->blur.radius = 0.0f;
    }

    return filter;
}

enum sb_filter_type sb_filter_type(const sb_filter_t *filter)
{
    return filter->type;
}

float sb_filter_blur_radius(const sb_filter_t *filter)
{
    return filter->blur.radius;
}

void sb_filter_blur_set_radius(sb_filter_t *filter, float radius)
{
    filter->blur.radius = radius;
}

#ifdef __cplusplus
}
#endif
