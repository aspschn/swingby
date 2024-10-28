#ifndef _08_WINDOWING_TITLE_BAR_H
#define _08_WINDOWING_TITLE_BAR_H

#include <swingby/swingby.h>

#define TITLE_BAR_HEIGHT 30
#define TITLE_BAR_BUTTON_SIZE 24

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

struct window;

struct title_bar {
    sb_view_t *view;
    float height;
    sb_view_t *close_button;
    sb_view_t *minimize_button;
    sb_view_t *maximize_restore_button;
    struct window *window;
};

struct title_bar* title_bar_new(sb_view_t *parent);

void title_bar_set_window(struct title_bar *title_bar, struct window *window);

// Stop propagation.
void on_button_press(sb_event_t *event);

void on_close_button_click(sb_event_t *event);
void on_close_button_pointer_enter(sb_event_t *event);
void on_close_button_pointer_leave(sb_event_t *event);

void on_minimize_button_click(sb_event_t *event);

void on_maximize_restore_button_click(sb_event_t *event);

//!<===================
//!< Button Colours
//!<===================

sb_color_t close_button_color();
sb_color_t close_button_color_hover();

sb_color_t minimize_button_color();
sb_color_t minimize_button_color_hover();

//!<===================
//!< Update
//!<===================

void title_bar_set_geometry(struct title_bar *title_bar, sb_rect_t geometry);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* _08_WINDOWING_TITLE_BAR_H */
