#ifndef _FOUNDATION_POINT_H
#define _FOUNDATION_POINT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct sb_point_t {
    float x;
    float y;
} sb_point_t;

typedef struct sb_point_i_t {
    uint64_t x;
    uint64_t y;
} sb_point_i_t;

#ifdef __cplusplus
}
#endif

#endif /* _FOUNDATION_POINT_H */
