#ifndef SWINGBY_RECT_H
#define SWINGBY_RECT_H

#include <stdbool.h>

#include <swingby/common.h>
#include <swingby/point.h>
#include <swingby/size.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct sb_rect_t {
    sb_point_t position;
    sb_size_t size;
} sb_rect_t;

typedef struct sb_rect_i_t {
    sb_point_i_t position;
    sb_size_i_t size;
} sb_rect_i_t;

typedef struct sb_radii_t {
    float top_left;
    float top_right;
    float bottom_right;
    float bottom_left;
} sb_radii_t;

typedef struct sb_rounded_rect_t {
    sb_point_t position;
    sb_size_t size;
    sb_radii_t radii;
} sb_rounded_rect_t;


sb_rect_t sb_rect_make(float x, float y, float width, float height);

sb_rect_i_t sb_rect_to_rect_i(sb_rect_t rect);

SB_EXPORT
bool sb_rect_contains_point(sb_rect_t *rect, const sb_point_t *point);

SB_EXPORT
bool sb_rect_intersects(const sb_rect_t *rect, const sb_rect_t *other);

SB_EXPORT
bool sb_rect_equals(const sb_rect_t *rect, const sb_rect_t *other);


sb_rect_i_t sb_rect_i_make(uint32_t x, uint32_t y, uint32_t w, uint32_t h);

sb_rect_t sb_rect_i_to_rect(sb_rect_i_t rect);


inline static sb_rounded_rect_t sb_rounded_rect_make_all(float x,
                                                         float y,
                                                         float width,
                                                         float height,
                                                         float radius)
{
    sb_rounded_rect_t rrect;
    rrect.position.x = x;
    rrect.position.y = y;
    rrect.size.width = width;
    rrect.size.height = height;
    rrect.radii.top_left = radius;
    rrect.radii.top_right = radius;
    rrect.radii.bottom_right = radius;
    rrect.radii.bottom_left = radius;
    return rrect;
}

#ifdef __cplusplus
}
#endif

#endif /* SWINGBY_RECT_H */
