#ifndef _08_WINDOWING_DECORATION_H
#define _08_WINDOWING_DECORATION_H

#include <swingby/swingby.h>

#include "title-bar.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

struct window;

struct decoration {
    /// The window of this decoration.
    struct window *window;
    /// Actual view itself.
    sb_view_t *view;
    struct {
        sb_view_t *view;
        float thickness;
        sb_size_t size;
    } shadow;
    struct {
        sb_view_t *view;
        float thickness;
        sb_size_t size;
    } border;
    struct {
        sb_view_t *view;
        float thickness;
        sb_size_t size;
    } resize;
    struct title_bar *title_bar;
};

struct decoration* decoration_new(struct window *window);

sb_size_t decoration_size(struct decoration *decoration, struct window *window);

void decoration_update_size(struct decoration *decoration);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* _08_WINDOWING_DECORATION_H */
