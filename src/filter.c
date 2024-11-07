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

    if (type == SB_FILTER_TYPE_DROP_SHADOW) {
        filter->drop_shadow.offset.x = 0.0f;
        filter->drop_shadow.offset.y = 0.0f;
        filter->drop_shadow.radius = 0.0f;
        sb_color_t color = { 0, 0, 0, 255 };    // Black colour for default.
        filter->drop_shadow.color = color;
    }

    return filter;
}

enum sb_filter_type sb_filter_type(const sb_filter_t *filter)
{
    return filter->type;
}

//!<=============
//!< Blur
//!<=============

float sb_filter_blur_radius(const sb_filter_t *filter)
{
    return filter->blur.radius;
}

void sb_filter_blur_set_radius(sb_filter_t *filter, float radius)
{
    filter->blur.radius = radius;
}

//!<==============
//!< Drop Shadow
//!<==============

const sb_point_t* sb_filter_drop_shadow_offset(const sb_filter_t *filter)
{
    return &filter->drop_shadow.offset;
}

void sb_filter_drop_shadow_set_offset(sb_filter_t *filter,
                                      const sb_point_t *offset)
{
    filter->drop_shadow.offset = *offset;
}

float sb_filter_drop_shadow_radius(const sb_filter_t *filter)
{
    return filter->drop_shadow.radius;
}

void sb_filter_drop_shadow_set_radius(sb_filter_t *filter, float radius)
{
    filter->drop_shadow.radius = radius;
}

const sb_color_t* sb_filter_drop_shadow_color(const sb_filter_t *filter)
{
    return &filter->drop_shadow.color;
}

void sb_filter_drop_shadow_set_color(sb_filter_t *filter,
                                     const sb_color_t *color)
{
    filter->drop_shadow.color = *color;
}

#ifdef __cplusplus
}
#endif
