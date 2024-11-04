#ifndef _SWINGBY_FILTER_H
#define _SWINGBY_FILTER_H

#include <stdbool.h>
#include <stdint.h>

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

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* _SWINGBY_FILTER_H */
