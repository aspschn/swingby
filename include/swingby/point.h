#ifndef _FOUNDATION_POINT_H
#define _FOUNDATION_POINT_H

#include <stdbool.h>
#include <stdint.h>

#include <swingby/common.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct sb_point_t {
    float x;
    float y;
} sb_point_t;

typedef struct sb_point_i_t {
    int64_t x;
    int64_t y;
} sb_point_i_t;

SB_EXPORT
float sb_point_x(const sb_point_t *point);

SB_EXPORT
float sb_point_y(const sb_point_t *point);

SB_EXPORT
bool sb_point_equals(const sb_point_t *point, const sb_point_t *other);

#ifdef __cplusplus
}
#endif

#endif /* _FOUNDATION_POINT_H */
