#include "converts.h"

SkColor4f sb_color_to_SkColor4f(sb_color_t color)
{
    SkColor4f skColor;
    skColor.fR = color.r;
    skColor.fG = color.g;
    skColor.fB = color.b;
    skColor.fA = color.a;
    return skColor;
}

SkRect sb_rect_to_SkRect(sb_rect_t rect, float scale)
{
    SkRect skRect = SkRect::MakeXYWH(
        rect.position.x * scale,
        rect.position.y * scale,
        rect.size.width * scale,
        rect.size.height * scale
    );
    return skRect;
}

SkRRect sb_rounded_rect_to_SkRRect(sb_rounded_rect_t rrect, float scale)
{
    SkRRect skRRect;

    SkRect skRect = SkRect::MakeXYWH(
        rrect.position.x * scale,
        rrect.position.y * scale,
        rrect.size.width * scale,
        rrect.size.height * scale
    );
    SkVector radii[] = {
        { rrect.radii.top_left * scale, rrect.radii.top_left * scale },
        { rrect.radii.top_right * scale, rrect.radii.top_right * scale },
        { rrect.radii.bottom_right * scale, rrect.radii.bottom_right * scale },
        { rrect.radii.bottom_left * scale, rrect.radii.bottom_left * scale },
    };
    skRRect.setRectRadii(skRect, radii);
    return skRRect;
}

SkPaint sb_paint_to_SkPaint(const sb_paint_t *paint)
{
    SkPaint skPaint;
    // TODO
    return skPaint;
}