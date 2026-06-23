#ifndef SWINGBY_PRIVATE_POINTER_PRIV_H
#define SWINGBY_PRIVATE_POINTER_PRIV_H

#include <stdint.h>
#include <stdbool.h>

#include <wayland-client.h>
#include <wayland-protocols/staging/cursor-shape-v1.h>

#include <swingby/point.h>
#include <swingby/input.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct sb_pointer_priv_t sb_pointer_priv_t;

typedef struct sb_application_t sb_application_t;
typedef struct sb_view_t sb_view_t;
typedef struct sb_cursor_t sb_cursor_t;

struct sb_pointer_priv_t {
    sb_application_t *sb_application;
    struct wl_pointer *wl_pointer;
    struct wp_cursor_shape_manager_v1 *wp_cursor_shape_manager_v1;
    /// \brief Current pointer surface.
    ///
    /// Pointer motion handler not pass `struct wl_surface` object.
    /// Store this information when pointer entered to the surface.
    struct wl_surface *current_wl_surface;
    /// \brief Current pointer view.
    ///
    /// Store the position of the view under the pointer.
    /// Usefull when check differences in motion event.
    sb_view_t *view;
    /// \brief Pointer enter event information.
    ///
    /// Currently used only when change cursor shape.
    uint32_t enter_serial;
    /// \brief Unused.
    uint32_t leave_serial;
    /// \brief Pointer button event serial.
    uint32_t button_serial;
    /// \brief Pointer event position.
    ///
    /// Pointer button and axis event not pass the position.
    /// Store this information when pointer moved.
    sb_point_t position;
    /// \brief Fallback cursor for no cursor shape protocol.
    sb_cursor_t *cursor;
    /// \brief Click event information.
    struct {
        sb_view_t *view;
        sb_pointer_button button;
    } click;
    /// \brief Double click event information.
    struct {
        sb_view_t *view;
        uint32_t click_count;
        uint32_t time;
        sb_pointer_button button;
    } double_click;
    struct {
        enum sb_pointer_scroll_axis axis;
        enum sb_pointer_scroll_source source;
        float value;
        /// \brief Vertical stop.
        bool ver_stop;
        /// \brief Horizontal stop.
        bool hor_stop;
        enum sb_pointer_scroll_axis stop_axis;
        bool frame;
    } scroll;
};

void sb_pointer_priv_init(sb_pointer_priv_t *pointer);

void sb_pointer_priv_add_listener(
    sb_pointer_priv_t *pointer, struct wl_seat *wl_seat);

#ifdef __cplusplus
}
#endif

#endif /* SWINGBY_PRIVATE_POINTER_PRIV_H */
