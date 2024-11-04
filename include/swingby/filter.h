#ifndef _SWINGBY_FILTER_H
#define _SWINGBY_FILTER_H

#include <stdbool.h>
#include <stdint.h>

#include <swingby/point.h>
#include <swingby/color.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

enum sb_filter_type {
    SB_FILTER_TYPE_BLUR,
    SB_FILTER_TYPE_DROP_SHADOW,
};

typedef struct sb_filter_t sb_filter_t;

struct sb_filter_t {
    enum sb_filter_type type;
    struct {
        float radius;
    } blur;
    struct {
        sb_point_t offset;
        float radius;
        sb_color_t color;
    } drop_shadow;
};

sb_filter_t* sb_filter_new(enum sb_filter_type type);

/// \brief Get the type of the filter.
enum sb_filter_type sb_filter_type(const sb_filter_t *filter);

/// \brief Get the blur filter radius.
///
/// If the filter type is not blur, return value is undefined.
float sb_filter_blur_radius(const sb_filter_t *filter);

/// \brief Set the blur filter radius.
///
/// Meaningless if the type is not blur.
void sb_filter_blur_set_radius(sb_filter_t *filter, float radius);

/// \brief Get the drop shadow filter offset.
const sb_point_t* sb_filter_drop_shadow_offset(const sb_filter_t *filter);

/// \brief Set the drop shadow filter offset.
void sb_filter_drop_shadow_set_offset(sb_filter_t *filter,
                                      const sb_point_t *offset);

/// \brief Get the drop shadow filter radius.
float sb_filter_drop_shadow_radius(const sb_filter_t *filter);

/// \brief Set the drop shadow filter radius.
void sb_filter_drop_shadow_set_radius(sb_filter_t *filter, float radius);

/// \brief Get the drop shadow filter color.
const sb_color_t* sb_filter_drop_shadow_color(const sb_filter_t *filter);

/// \brief Set the drop shadow filter color.
void sb_filter_drop_shadow_set_color(sb_filter_t *filter,
                                     const sb_color_t *color);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* _SWINGBY_FILTER_H */
