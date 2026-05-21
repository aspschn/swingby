#include <swingby/font.h>

#include <stdlib.h>

#include <skia/include/core/SkTypeface.h>
#include <skia/include/core/SkFontMgr.h>
#include <skia/include/core/SkFont.h>
#include <skia/include/core/SkFontMetrics.h>
#include <skia/include/ports/SkFontMgr_directory.h>

#include <swingby/rect.h>
#include <swingby/log.h>

#ifdef __cplusplus
extern "C" {
#endif

sb_font_metrics_t* sb_font_metrics_new(const sb_font_t *font)
{
    sb_font_metrics_t *metrics =
        (sb_font_metrics_t*)malloc(sizeof(sb_font_metrics_t));

    sk_sp<SkFontMgr> manager = SkFontMgr_New_Custom_Directory("/usr/share/fonts/");
    sk_sp<SkTypeface> typeface = manager->makeFromFile(
        font->path,
        font->ttc_index
    );
    SkFont sk_font = SkFont(typeface, font->size);
    SkFontMetrics sk_metrics;
    sk_font.getMetrics(&sk_metrics);

    metrics->ascent = sk_metrics.fAscent;
    metrics->descent = sk_metrics.fDescent;
    metrics->leading = sk_metrics.fLeading;
    metrics->cap_height = sk_metrics.fCapHeight;
    metrics->x_height = sk_metrics.fXHeight;

    return metrics;
}

void sb_font_metrics_free(sb_font_metrics_t *metrics)
{
    free(metrics);
}

#ifdef __cplusplus
}
#endif
