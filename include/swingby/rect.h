#ifndef _FOUNDATION_RECT_H
#define _FOUNDATION_RECT_H

#include <stdbool.h>

#include <swingby/common.h>
#include <swingby/point.h>
#include <swingby/size.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct sb_rect_t {
    sb_point_t pos;
    sb_size_t size;
} sb_rect_t;

typedef struct sb_rect_i_t {
    sb_point_i_t pos;
    sb_size_i_t size;
} sb_rect_i_t;

SB_EXPORT
bool sb_rect_contains_point(sb_rect_t *rect, const sb_point_t *point);

SB_EXPORT
bool sb_rect_intersects(const sb_rect_t *rect, const sb_rect_t *other);

SB_EXPORT
bool sb_rect_equals(const sb_rect_t *rect, const sb_rect_t *other);

#ifdef __cplusplus
}
#endif

#endif /* _FOUNDATION_RECT_H */
