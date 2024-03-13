#ifndef _FOUNDATION_RECT_H
#define _FOUNDATION_RECT_H

#include <stdbool.h>

#include <foundation/point.h>
#include <foundation/size.h>

typedef struct ft_rect_t {
    ft_point_t pos;
    ft_size_t size;
} ft_rect_t;

bool ft_rect_contains_point(ft_rect_t *rect, const ft_point_t *point);

#endif /* _FOUNDATION_RECT_H */
