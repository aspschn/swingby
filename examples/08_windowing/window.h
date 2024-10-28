#ifndef _08_WINDOWING_WINDOW_H
#define _08_WINDOWING_WINDOW_H

#include <swingby/swingby.h>

#include "decoration.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

struct window {
    sb_desktop_surface_t *desktop_surface;
    struct decoration *decoration;
    sb_view_t *body;
};

/// New window with body size.
struct window* window_new(sb_size_t size);

void window_show(struct window *window);


void on_desktop_surface_resize(sb_event_t *event);

//!<============
//!< Sizes
//!<============

// Only body size.
sb_size_t window_body_size(struct window *window);
// Visible frame size. Border and title bar.
sb_size_t window_frame_size(struct window *window);
// Whole window surface size. Includes the shadow.
sb_size_t window_whole_size(struct window *window);

void window_set_surface_size(struct window *window, sb_size_t size);

//!<============
//!< Geometry
//!<============

sb_rect_t window_body_geometry(struct window *window);

sb_rect_t window_frame_geometry(struct window *window);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* _08_WINDOWING_WINDOW_H */
