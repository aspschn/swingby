#include <swingby/canvas.h>

#include <stdlib.h>

#include <skia/include/core/SkCanvas.h>

#include <swingby/rect.h>

#ifdef __cplusplus
extern "C" {
#endif

struct sb_canvas_t {
    SkCanvas *sk_canvas;
    sb_paint_t paint;
    float scale;
};

sb_canvas_t* sb_canvas_new(void *sk_canvas)
{
    sb_canvas_t *canvas = (sb_canvas_t*)malloc(sizeof(sb_canvas_t));

    canvas->sk_canvas = (SkCanvas*)sk_canvas;
    canvas->paint.fill_color = sb_color_t { .0f, .0f, .0f, .0f };
    canvas->scale = 1.0f;

    return canvas;
}

void sb_canvas_set_scale(sb_canvas_t *canvas, float scale)
{
    canvas->scale = scale;
}

sb_paint_t* sb_canvas_paint(sb_canvas_t *canvas)
{
    return &canvas->paint;
}

void sb_canvas_draw_rect(sb_canvas_t *canvas,
                         const sb_rect_t *rect,
                         const sb_paint_t *paint)
{
    const float scale = canvas->scale;

    SkRect sk_rect = SkRect::MakeXYWH(
        rect->pos.x * scale, rect->pos.y * scale,
        rect->size.width * scale, rect->size.height * scale);

    SkPaint sk_paint;

    SkColor4f color;
    color.fR = paint->fill_color.r;
    color.fG = paint->fill_color.g;
    color.fB = paint->fill_color.b;
    color.fA = paint->fill_color.a;

    sk_paint.setColor4f(color);

    canvas->sk_canvas->drawRect(sk_rect, sk_paint);
}

void sb_canvas_free(sb_canvas_t *canvas)
{
    free(canvas);
}

#ifdef __cplusplus
}
#endif
