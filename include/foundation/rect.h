#ifndef _FOUNDATION_RECT_H
#define _FOUNDATION_RECT_H

#include <stdbool.h>

#include <foundation/point.h>
#include <foundation/size.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ft_rect_t {
    ft_point_t pos;
    ft_size_t size;
} ft_rect_t;

typedef struct ft_rect_i_t {
    ft_point_i_t pos;
    ft_size_i_t size;
} ft_rect_i_t;

bool ft_rect_contains_point(ft_rect_t *rect, const ft_point_t *point);

#ifdef __cplusplus
}
#endif

#endif /* _FOUNDATION_RECT_H */
