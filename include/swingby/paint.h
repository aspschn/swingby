#ifndef _SWINGBY_PAINT_H
#define _SWINGBY_PAINT_H

#include <swingby/color.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef struct sb_paint_t sb_paint_t;

struct sb_paint_t {
    sb_color_t fill_color;
    sb_color_t stroke_color;
    float stroke_width;
};

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* _SWINGBY_PAINT_H */
