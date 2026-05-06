#ifndef _SWINGBY_FONT_H
#define _SWINGBY_FONT_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef struct sb_font_t sb_font_t;

struct sb_font_t {
    const char *path;
    int ttc_index;
    float size;
};

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* _SWINGBY_FONT_H */
