#ifndef _FOUNDATION_POINT_H
#define _FOUNDATION_POINT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ft_point_t {
    float x;
    float y;
} ft_point_t;

typedef struct ft_point_i_t {
    uint64_t x;
    uint64_t y;
} ft_point_i_t;

#ifdef __cplusplus
}
#endif

#endif /* _FOUNDATION_POINT_H */
