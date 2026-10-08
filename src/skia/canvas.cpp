#include <swingby/canvas.h>

#include <stdlib.h>

#include <skia/include/core/SkCanvas.h>
#include <skia/include/core/SkRRect.h>

#include <swingby/rect.h>

#include "converts.h"
#include "include/core/SkPaint.h"

#ifdef __cplusplus
extern "C" {
#endif

struct sb_canvas_t {
    SkCanvas *sk_canvas;
    sb_paint_t *paint;
    sb_point_t position;
    float scale;
};

sb_canvas_t* sb_canvas_new(void *sk_canvas)
{
    sb_canvas_t *canvas = (sb_canvas_t*)malloc(sizeof(sb_canvas_t));

    canvas->sk_canvas = (SkCanvas*)sk_canvas;

    canvas->paint = sb_paint_new();
    sb_color_t fill_color = sb_color_t { .0f, .0f, .0f, .0f };
    sb_color_t stroke_color = sb_color_t { .0f, .0f, .0f, .0f };
    sb_paint_set_fill_color(canvas->paint, &fill_color);
    sb_paint_set_stroke_color(canvas->paint, &stroke_color);
    sb_paint_set_stroke_width(canvas->paint, 0.0f);

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

void sb_canvas_clear(sb_canvas_t *canvas, sb_color_t color)
{
    canvas->sk_canvas->saveLayer(nullptr, nullptr);
    canvas->sk_canvas->clear(sb_color_to_SkColor4f(color));
    canvas->sk_canvas->restore();
}

sb_paint_t* sb_canvas_paint(sb_canvas_t *canvas)
{
    return canvas->paint;
}

void sb_canvas_clip_rect(sb_canvas_t *canvas, sb_rect_t rect)
{
    SkRect sk_rect = sb_rect_to_SkRect(rect, canvas->scale);
    canvas->sk_canvas->clipRect(sk_rect);
}

void sb_canvas_clip_rounded_rect(sb_canvas_t *canvas, sb_rounded_rect_t rrect)
{
    SkRRect sk_rrect = sb_rounded_rect_to_SkRRect(rrect, canvas->scale);
    canvas->sk_canvas->clipRRect(sk_rrect, true);
}

void sb_canvas_draw_rect(sb_canvas_t *canvas,
                         const sb_rect_t *rect,
                         const sb_paint_t *paint)
{
    const float scale = canvas->scale;

    SkRect sk_rect = SkRect::MakeXYWH(
        roundf((rect->position.x + canvas->position.x) * scale),
        roundf((rect->position.y + canvas->position.y) * scale),
        roundf(rect->size.width * scale),
        roundf(rect->size.height * scale)
    );

    SkPaint sk_paint;

    const sb_color_t *fill_color = sb_paint_fill_color(paint);
    SkColor4f color = sb_color_to_SkColor4f(*fill_color);

    sk_paint.setColor4f(color);

    canvas->sk_canvas->drawRect(sk_rect, sk_paint);
}

void sb_canvas_draw_rounded_rect(sb_canvas_t *canvas,
                                 sb_rounded_rect_t rrect,
                                 const sb_paint_t *paint)
{
    const float scale = canvas->scale;

    SkRect sk_rect = SkRect::MakeXYWH(
        (rrect.position.x + canvas->position.x) * scale,
        (rrect.position.y + canvas->position.y) * scale,
        rrect.size.width * scale,
        rrect.size.height * scale
    );

    SkRRect sk_rrect;
    SkVector radii[] = {
        { rrect.radii.top_left * scale, rrect.radii.top_left * scale },
        { rrect.radii.top_right * scale, rrect.radii.top_right * scale },
        { rrect.radii.bottom_right * scale, rrect.radii.bottom_right * scale },
        { rrect.radii.bottom_left * scale, rrect.radii.bottom_left * scale },
    };

    sk_rrect.setRectRadii(sk_rect, radii);

    SkPaint sk_paint;

    // Draw fill.
    const sb_color_t *fill_color = sb_paint_fill_color(paint);
    SkColor4f color = sb_color_to_SkColor4f(*fill_color);

    sk_paint.setColor4f(color);

    canvas->sk_canvas->drawRRect(sk_rrect, sk_paint);

    if (sb_paint_stroke_width(paint) == 0.0) {
        return;
    }
    // Set stroke.
    float stroke_width = sb_paint_stroke_width(paint);
    sk_paint.setStyle(SkPaint::Style::kStroke_Style);
    sk_paint.setStrokeWidth(stroke_width);
    const sb_color_t *stroke_color = sb_paint_stroke_color(paint);
    sk_paint.setColor4f(sb_color_to_SkColor4f(*stroke_color));

    // Apply sizing policy and draw.
    enum sb_stroke_sizing sizing = sb_paint_stroke_sizing(paint);
    switch (sizing) {
    case SB_STROKE_SIZING_INNER:
        sk_rrect.inset(stroke_width / 2, stroke_width / 2);
        canvas->sk_canvas->drawRRect(sk_rrect, sk_paint);
        break;
    case SB_STROKE_SIZING_CENTER:
    case SB_STROKE_SIZING_OUTER:    // TODO: Not implemented.
        canvas->sk_canvas->drawRRect(sk_rrect, sk_paint);
        break;
    }
}

void sb_canvas_draw_line(sb_canvas_t *canvas,
                         const sb_point_t *p1,
                         const sb_point_t *p2,
                         const sb_paint_t *paint)
{
    const float scale = canvas->scale;

    SkPaint sk_paint;

    SkColor4f color;
    const sb_color_t *stroke_color = sb_paint_stroke_color(paint);
    color.fR = stroke_color->r;
    color.fG = stroke_color->g;
    color.fB = stroke_color->b;
    color.fA = stroke_color->a;

    sk_paint.setStyle(SkPaint::Style::kStroke_Style);
    sk_paint.setColor4f(color);
    sk_paint.setStrokeWidth(sb_paint_stroke_width(paint) * scale);

    canvas->sk_canvas->drawLine(
        (p1->x + canvas->position.x) * scale,
        (p1->y + canvas->position.y) * scale,
        (p2->x + canvas->position.x) * scale,
        (p2->y + canvas->position.y) * scale,
        sk_paint
    );
}

void sb_canvas_save(sb_canvas_t *canvas)
{
    canvas->sk_canvas->save();
}

void sb_canvas_restore(sb_canvas_t *canvas)
{
    canvas->sk_canvas->restore();
}

void sb_canvas_free(sb_canvas_t *canvas)
{
    sb_paint_free(canvas->paint);
    free(canvas);
}

#ifdef __cplusplus
}
#endif
