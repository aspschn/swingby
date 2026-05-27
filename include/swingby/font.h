#ifndef _SWINGBY_FONT_H
#define _SWINGBY_FONT_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef struct sb_font_t sb_font_t;
typedef struct sb_font_metrics_t sb_font_metrics_t;

struct sb_font_t {
    const char *path;
    int ttc_index;
    float size;
};

struct sb_font_metrics_t {
    float ascent;
    float descent;
    float leading;
    float cap_height;
    float x_height;
};

sb_font_metrics_t* sb_font_metrics_new(const sb_font_t *font);

void sb_font_metrics_free(sb_font_metrics_t *metrics);

void* sb_font_font_mgr_instance();

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* _SWINGBY_FONT_H */
