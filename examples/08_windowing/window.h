#ifndef _08_WINDOWING_WINDOW_H
#define _08_WINDOWING_WINDOW_H

#include <stdbool.h>

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

void window_maximize(struct window *window);

void window_restore(struct window *window);


void on_desktop_surface_resize(sb_event_t *event);

void window_set_on_close_button_click(struct window *window,
                                      void (*handler)(sb_event_t*));

void window_set_on_maximize_restore_button_click(struct window *window,
                                                 void (*handler)(sb_event_t*));

void window_set_on_title_bar_press(struct window *window,
                                   void (*handler)(sb_event_t*));

void window_set_on_title_bar_release(struct window *window,
                                     void (*handler)(sb_event_t*));

void window_set_on_title_bar_pointer_move(struct window *window,
                                          void (*handler)(sb_event_t*));

void window_set_on_state_change(struct window *window,
                                void (*handler)(sb_event_t*));

//!<==============
//!< Properties
//!<==============

bool window_maximized(struct window *window);

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

/// Get the body size for the given window size.
sb_size_t window_body_size_for(struct window *window, sb_size_t size);

/// Get the surface size for the given window size.
sb_size_t window_surface_size_for(struct window *window, sb_size_t size);

//!<============
//!< Geometry
//!<============

sb_rect_t window_body_geometry(struct window *window);

sb_rect_t window_frame_geometry(struct window *window);

float window_decoration_border_offset(struct window *window);

float window_decoration_resize_offset(struct window *window);

float window_body_offset_x(struct window *window);

float window_body_offset_y(struct window *window);

float window_decoration_title_bar_offset(struct window *window);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* _08_WINDOWING_WINDOW_H */
