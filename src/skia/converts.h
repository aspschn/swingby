#ifndef SWINGBY_SKIA_CONVERTS_H
#define SWINGBY_SKIA_CONVERTS_H

#include <skia/include/core/SkColor.h>
#include <skia/include/core/SkRect.h>
#include <skia/include/core/SkRRect.h>
#include <skia/include/core/SkPaint.h>

#include <swingby/common.h>
#include <swingby/rect.h>
#include <swingby/color.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct sb_paint_t sb_paint_t;

#ifdef __cplusplus
}
#endif


// These functions are C++ functions. Disable in C for avoid mistakes.
#ifdef __cplusplus

SB_INTERNAL
SkColor4f sb_color_to_SkColor4f(sb_color_t color);

SB_INTERNAL
SkRect sb_rect_to_SkRect(sb_rect_t rect, float scale = 1.0f);

SB_INTERNAL
SkRRect sb_rounded_rect_to_SkRRect(sb_rounded_rect_t rrect, float scale = 1.0f);

SB_INTERNAL
SkPaint sb_paint_to_SkPaint(const sb_paint_t *paint);

#endif // __cplusplus

#endif /* SWINGBY_SKIA_CONVERTS_H */
