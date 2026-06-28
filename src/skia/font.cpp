#include <swingby/font.h>

#include <stdlib.h>

#include <utility>
#include <map>

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

static sk_sp<SkFontMgr> _font_mgr_instance = nullptr;

static std::map<std::pair<std::string, int>, sk_sp<SkTypeface>> _font_cache;

sb_font_metrics_t* sb_font_metrics_new(const sb_font_t *font)
{
    sb_font_metrics_t *metrics =
        (sb_font_metrics_t*)malloc(sizeof(sb_font_metrics_t));

    SkFontMgr *font_mgr = (SkFontMgr*)sb_font_font_mgr_instance();
    // By Skia's documentation,
    // "The caller must call unref() on the returned object if it is not null."
    // However, from commit `4bf296be2821d2bdd0afabae9fdfe18e7e9b59cb`,
    // it returns sk_sp rather than raw pointer. But document not changed.
    // This is now wrong information.
    sk_sp<SkTypeface> typeface = font_mgr->makeFromFile(
        font->path,
        font->ttc_index
    );
    SkFont sk_font = SkFont(typeface, font->size);
    SkFontMetrics sk_metrics;
    sk_font.getMetrics(&sk_metrics);

    metrics->ascent = -sk_metrics.fAscent;
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

void* sb_font_font_mgr_instance()
{
    if (_font_mgr_instance == nullptr) {
        _font_mgr_instance =
            SkFontMgr_New_Custom_Directory("");
    }

    return _font_mgr_instance.get();
}

void* sb_font_font_cache_find(const char *font_path, int ttc_index)
{
    std::string str = font_path;
    std::pair<std::string, int> key = std::make_pair(str, ttc_index);
    auto found = _font_cache.find(key);
    if (found == _font_cache.end()) {
        sk_sp<SkTypeface> typeface = _font_mgr_instance->makeFromFile(
            str.c_str(), ttc_index);
        if (typeface == nullptr) {
            return nullptr;
        }
        _font_cache[key] = typeface;
    }
    return (void*)(&_font_cache[key]);
}

#ifdef __cplusplus
}
#endif
