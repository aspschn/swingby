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
    sb_point_t position;
    float scale;
};

sb_canvas_t* sb_canvas_new(void *sk_canvas)
{
    sb_canvas_t *canvas = (sb_canvas_t*)malloc(sizeof(sb_canvas_t));

    canvas->sk_canvas = (SkCanvas*)sk_canvas;
    canvas->paint.fill_color = sb_color_t { .0f, .0f, .0f, .0f };
    canvas->paint.stroke_color = sb_color_t { .0f, .0f, .0f, .0f };
    canvas->paint.stroke_width = 0.0f;
    canvas->scale = 1.0f;

    return canvas;
}

void sb_canvas_set_scale(sb_canvas_t *canvas, float scale)
{
    canvas->scale = scale;
}

void sb_canvas_set_position(sb_canvas_t *canvas, const sb_point_t *position)
{
    canvas->position = *position;
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
        (rect->position.x + canvas->position.x) * scale,
        (rect->position.y + canvas->position.y) * scale,
        rect->size.width * scale,
        rect->size.height * scale
    );

    SkPaint sk_paint;

    SkColor4f color;
    color.fR = paint->fill_color.r;
    color.fG = paint->fill_color.g;
    color.fB = paint->fill_color.b;
    color.fA = paint->fill_color.a;

    sk_paint.setColor4f(color);

    canvas->sk_canvas->drawRect(sk_rect, sk_paint);
}

void sb_canvas_draw_line(sb_canvas_t *canvas,
                         const sb_point_t *p1,
                         const sb_point_t *p2,
                         const sb_paint_t *paint)
{
    const float scale = canvas->scale;

    SkPaint sk_paint;

    SkColor4f color;
    color.fR = paint->stroke_color.r;
    color.fG = paint->stroke_color.g;
    color.fB = paint->stroke_color.b;
    color.fA = paint->stroke_color.a;

    sk_paint.setStyle(SkPaint::Style::kStroke_Style);
    sk_paint.setColor4f(color);
    sk_paint.setStrokeWidth(paint->stroke_width * scale);

    canvas->sk_canvas->drawLine(
        (p1->x + canvas->position.x) * scale,
        (p1->y + canvas->position.y) * scale,
        (p2->x + canvas->position.x) * scale,
        (p2->y + canvas->position.y) * scale,
        sk_paint
    );
}

void sb_canvas_free(sb_canvas_t *canvas)
{
    free(canvas);
}

#ifdef __cplusplus
}
#endif
