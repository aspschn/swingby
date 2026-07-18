#ifndef _SWINGBY_PAINT_H
#define _SWINGBY_PAINT_H

#include <stdbool.h>

#include <swingby/color.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef struct sb_paint_t sb_paint_t;

sb_paint_t* sb_paint_new();

const sb_color_t* sb_paint_fill_color(const sb_paint_t *paint);

void sb_paint_set_fill_color(sb_paint_t *paint, const sb_color_t *color);

const sb_color_t* sb_paint_stroke_color(const sb_paint_t *paint);

void sb_paint_set_stroke_color(sb_paint_t *paint, const sb_color_t *color);

float sb_paint_stroke_width(const sb_paint_t *paint);

void sb_paint_set_stroke_width(sb_paint_t *paint, float width);

bool sb_paint_antialiasing(const sb_paint_t *paint);

void sb_paint_set_antialiasing(sb_paint_t *paint, bool value);

void sb_paint_free(sb_paint_t *paint);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* _SWINGBY_PAINT_H */
