#include <swingby/paint.h>

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

struct sb_paint_t {
    sb_color_t fill_color;
    sb_color_t stroke_color;
    float stroke_width;
    bool antialiasing;
};

sb_paint_t* sb_paint_new()
{
    sb_paint_t *paint = malloc(sizeof(sb_paint_t));

    paint->fill_color.r = 0.0f;
    paint->fill_color.g = 0.0f;
    paint->fill_color.b = 0.0f;
    paint->fill_color.a = 1.0f;

    paint->stroke_color.r = 1.0f;
    paint->stroke_color.g = 1.0f;
    paint->stroke_color.b = 1.0f;
    paint->stroke_color.a = 1.0f;

    paint->stroke_width = 0.0f;

    paint->antialiasing = false;

    return paint;
}

const sb_color_t* sb_paint_fill_color(const sb_paint_t *paint)
{
    return &paint->fill_color;
}

const sb_color_t* sb_paint_stroke_color(const sb_paint_t *paint)
{
    return &paint->stroke_color;
}

float sb_paint_stroke_width(const sb_paint_t *paint)
{
    return paint->stroke_width;
}

bool sb_paint_antialiasing(const sb_paint_t *paint)
{
    return paint->antialiasing;
}

void sb_paint_set_fill_color(sb_paint_t *paint, const sb_color_t *color)
{
    paint->fill_color = *color;
}

void sb_paint_set_stroke_color(sb_paint_t *paint, const sb_color_t *color)
{
    paint->stroke_color = *color;
}

void sb_paint_set_stroke_width(sb_paint_t *paint, float width)
{
    paint->stroke_width = width;
}

void sb_paint_set_antialiasing(sb_paint_t *paint, bool value)
{
    paint->antialiasing = value;
}

void sb_paint_free(sb_paint_t *paint)
{
    free(paint);
}

#ifdef __cplusplus
}
#endif
