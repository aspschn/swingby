#include "pointer-priv.h"

#include <linux/input.h>

#include <swingby/point.h>
#include <swingby/list.h>
#include <swingby/log.h>
#include <swingby/input.h>
#include <swingby/view.h>
#include <swingby/surface.h>
#include <swingby/desktop-surface.h>
#include <swingby/cursor.h>
#include <swingby/application.h>

#include "../helpers/shared.h"
#include "../helpers/application.h"

//!<====================
//!< Helper Functions
//!<====================

static void _reset_double_click(sb_pointer_priv_t *pointer)
{
    pointer->double_click.view = NULL;
    pointer->double_click.click_count = 0;
    pointer->double_click.time = 0;
    pointer->double_click.button = SB_POINTER_BUTTON_NONE;
}

static void _change_cursor_shape(sb_pointer_priv_t *pointer,
                                 struct wl_pointer *wl_pointer,
                                 enum sb_cursor_shape shape)
{
    if (pointer->wp_cursor_shape_manager_v1 != NULL) {
        struct wp_cursor_shape_device_v1 *device =
            wp_cursor_shape_manager_v1_get_pointer(
                pointer->wp_cursor_shape_manager_v1,
                wl_pointer
            );
        wp_cursor_shape_device_v1_set_shape(device,
            pointer->enter_serial,
            _to_wp_cursor_shape(shape));
        wp_cursor_shape_device_v1_destroy(device);
    }
}

/// \brief Linux button to Swingby pointer button.
static sb_pointer_button _from_linux_button(uint32_t button)
{
    switch (button) {
    case BTN_LEFT:
        return SB_POINTER_BUTTON_LEFT;
    case BTN_RIGHT:
        return SB_POINTER_BUTTON_RIGHT;
    case BTN_MIDDLE:
        return SB_POINTER_BUTTON_MIDDLE;
    default:
        return SB_POINTER_BUTTON_UNIMPLEMENTED;
    }
}

static void _post_pointer_enter_event(sb_view_t *view,
                                      float x,
                                      float y)
{
    sb_event_t *event = sb_event_new(SB_EVENT_TARGET_TYPE_VIEW,
        view,
        SB_EVENT_TYPE_POINTER_ENTER);

    event->pointer.button = SB_POINTER_BUTTON_NONE;
    event->pointer.position.x = x;
    event->pointer.position.y = y;

    sb_application_post_event(sb_application_instance(), event);
}

static void _post_pointer_leave_event(sb_view_t *view,
                                      float x,
                                      float y)
{
    sb_event_t *event = sb_event_new(SB_EVENT_TARGET_TYPE_VIEW,
        view,
        SB_EVENT_TYPE_POINTER_LEAVE);

    event->pointer.button = SB_POINTER_BUTTON_NONE;
    // TODO: How to get pointer leave position?
    event->pointer.position.x = x;
    event->pointer.position.y = y;

    sb_application_post_event(sb_application_instance(), event);
}

//!<============
//!< Pointer
//!<============

static void pointer_enter_handler(void *data,
                                  struct wl_pointer *wl_pointer,
                                  uint32_t serial,
                                  struct wl_surface *wl_surface,
                                  wl_fixed_t sx,
                                  wl_fixed_t sy)
{
    sb_pointer_priv_t *pointer = (sb_pointer_priv_t*)data;

    pointer->current_wl_surface = wl_surface;

    // Set the serial.
    pointer->enter_serial = serial;

    // Cursor shape.
    if (pointer->wp_cursor_shape_manager_v1 != NULL) {
        struct wp_cursor_shape_device_v1 *device =
            wp_cursor_shape_manager_v1_get_pointer(
                pointer->wp_cursor_shape_manager_v1,
                wl_pointer
            );
        wp_cursor_shape_device_v1_set_shape(device, serial,
            WP_CURSOR_SHAPE_DEVICE_V1_SHAPE_DEFAULT);

        wp_cursor_shape_device_v1_destroy(device);
    } else {
        // TEST cursor.
        // Set default cursor.
        if (pointer->cursor == NULL) {
            sb_point_t hot_spot;
            hot_spot.x = 0;
            hot_spot.y = 0;
            pointer->cursor = sb_cursor_new(SB_CURSOR_SHAPE_DEFAULT, &hot_spot);
        }

        sb_surface_t *cursor_surface = sb_cursor_surface(pointer->cursor);
        wl_pointer_set_cursor(wl_pointer,
            serial, sb_surface_wl_surface(cursor_surface), 0, 0);
    }

    float x = wl_fixed_to_double(sx);
    float y = wl_fixed_to_double(sy);

    // Store the pointer position.
    // Since while resizing the desktop surface, there is no motion event.
    // Instead the surface enter event is fired. So this is important to
    // re-assign pointer position when enter event.
    //
    // Remember: The pointer enter event could be fired even mouse pointer is
    // already in the surface area.
    pointer->position.x = x;
    pointer->position.y = y;

    // Find the surface.
    sb_surface_t *found = sb_application_find_surface_by_wl_surface(
        pointer->sb_application, wl_surface);

    // Make an event.
    sb_event_t *event = sb_event_new(SB_EVENT_TARGET_TYPE_SURFACE,
        (void*)found,
        SB_EVENT_TYPE_POINTER_ENTER);
    event->pointer.button = SB_POINTER_BUTTON_NONE;
    event->pointer.position.x = x;
    event->pointer.position.y = y;

    // Post the event.
    sb_application_post_event(pointer->sb_application, event);

    // Find most child.
    sb_view_t *root_view = sb_surface_root_view(found);
    sb_point_t position;
    position.x = x;
    position.y = y;
    sb_log_debug(" == root view: %p ==\n", root_view);
    sb_view_t *view = _find_most_child(root_view, &position);

    pointer->view = view;

    // Change cursor shape.
    _change_cursor_shape(pointer, wl_pointer, sb_view_cursor_shape(view));

    // Post the event (view).
    _post_pointer_enter_event(view, position.x, position.y);
}

static void pointer_leave_handler(void *data,
                                  struct wl_pointer *wl_pointer,
                                  uint32_t serial,
                                  struct wl_surface *surface)
{
    sb_pointer_priv_t *pointer = (sb_pointer_priv_t*)data;

    pointer->leave_serial = serial;
}

static void pointer_motion_handler(void *data,
                                   struct wl_pointer *wl_pointer,
                                   uint32_t time,
                                   wl_fixed_t sx,
                                   wl_fixed_t sy)
{
    sb_pointer_priv_t *pointer = (sb_pointer_priv_t*)data;

    float x = wl_fixed_to_double(sx);
    float y = wl_fixed_to_double(sy);

    // Store the position.
    pointer->position.x = x;
    pointer->position.y = y;

    // Find the surface.
    sb_surface_t *surface = sb_application_find_surface_by_wl_surface(
        pointer->sb_application, pointer->current_wl_surface);

    // Find most child view.
    sb_point_t pos;
    pos.x = x;
    pos.y = y;
    sb_view_t *view = _find_most_child(sb_surface_root_view(surface), &pos);

    // Pointer move event.
    {
        sb_event_t *move_event = sb_event_new(SB_EVENT_TARGET_TYPE_VIEW,
            (void*)view,
            SB_EVENT_TYPE_POINTER_MOVE);
        move_event->pointer.button = SB_POINTER_BUTTON_NONE;
        move_event->pointer.position.x = pos.x;
        move_event->pointer.position.y = pos.y;

        sb_application_post_event(pointer->sb_application, move_event);
    }

    // Check difference.
    if (view != pointer->view) {
        // Post the leave event for the previous view.
        // TODO: Leave position.
        _post_pointer_leave_event(pointer->view, 0.0f, 0.0f);

        pointer->view = view;

        // Cursor shape.
        _change_cursor_shape(pointer, wl_pointer, sb_view_cursor_shape(view));

        // Post the event.
        _post_pointer_enter_event(view, pos.x, pos.y);
    }
}

static void pointer_button_handler(void *data,
                                   struct wl_pointer *wl_pointer,
                                   uint32_t serial,
                                   uint32_t time,
                                   uint32_t button,
                                   uint32_t state)
{
    sb_pointer_priv_t *pointer = (sb_pointer_priv_t*)data;

    pointer->button_serial = serial;

    float x = pointer->position.x;
    float y = pointer->position.y;

    // Find the surface.
    sb_surface_t *surface = sb_application_find_surface_by_wl_surface(
        pointer->sb_application, pointer->current_wl_surface);

    // Find most child view.
    sb_point_t pos = { .x = x, .y = y };
    sb_view_t *view = _find_most_child(sb_surface_root_view(surface), &pos);

    // Set the event type.
    enum sb_event_type evt_type = SB_EVENT_TYPE_POINTER_PRESS;
    if (state == WL_POINTER_BUTTON_STATE_PRESSED) {
        evt_type = SB_EVENT_TYPE_POINTER_PRESS;
    } else if (state == WL_POINTER_BUTTON_STATE_RELEASED) {
        evt_type = SB_EVENT_TYPE_POINTER_RELEASE;
    }
    sb_event_t *event = sb_event_new(SB_EVENT_TARGET_TYPE_VIEW,
        (void*)view,
        evt_type);

    event->pointer.button = _from_linux_button(button);
    event->pointer.position = pos;

    // Post the event.
    sb_application_post_event(pointer->sb_application, event);

    // Click event.
    if (state == WL_POINTER_BUTTON_STATE_PRESSED) {
        pointer->click.view = view;
        pointer->click.button = _from_linux_button(button);
    } else if (state == WL_POINTER_BUTTON_STATE_RELEASED) {
        if (view == pointer->click.view) {
            sb_event_t *click_event = sb_event_new(SB_EVENT_TARGET_TYPE_VIEW,
                (void*)view,
                SB_EVENT_TYPE_POINTER_CLICK);
            click_event->pointer.button = pointer->click.button;
            click_event->pointer.position = pos;

            pointer->click.view = NULL;

            sb_application_post_event(pointer->sb_application, click_event);

            // Double click.
            pointer->double_click.click_count += 1;

            if (pointer->double_click.view == NULL) {
                pointer->double_click.view = view;
                pointer->double_click.button = _from_linux_button(button);
            }
            // Reset double click info if different view or button.
            if (pointer->double_click.view != view &&
                pointer->double_click.button != _from_linux_button(button)) {
                _reset_double_click(pointer);
            }
            // Store time if click count is 1.
            if (pointer->double_click.click_count == 1) {
                pointer->double_click.time = time;
            }
            if (pointer->double_click.click_count == 2 &&
                pointer->double_click.view == view) {
                uint32_t diff = time - pointer->double_click.time;
                if (diff <= 1000) {
                    sb_log_debug("DOUBLE CLICK! %p\n", view);
                    sb_event_t *dbl_click_event = sb_event_new(
                        SB_EVENT_TARGET_TYPE_VIEW,
                        view,
                        SB_EVENT_TYPE_POINTER_DOUBLE_CLICK);
                    sb_application_post_event(pointer->sb_application,
                        dbl_click_event);
                }
                _reset_double_click(pointer);
            }
            if (pointer->double_click.click_count > 2) {
                _reset_double_click(pointer);
            }
        }
    }
}

static void pointer_axis_handler(void *data,
                                 struct wl_pointer *wl_pointer,
                                 uint32_t time,
                                 uint32_t axis,
                                 wl_fixed_t value)
{
    sb_pointer_priv_t *pointer = (sb_pointer_priv_t*)data;

    float val = wl_fixed_to_double(value);
    sb_log_debug("pointer_axis_handler() - value: %.2f, axis: %d\n", val, axis);

    enum sb_pointer_scroll_axis sb_axis =
        SB_POINTER_SCROLL_AXIS_VERTICAL_SCROLL;
    switch (axis) {
    case WL_POINTER_AXIS_VERTICAL_SCROLL:
        sb_axis = SB_POINTER_SCROLL_AXIS_VERTICAL_SCROLL;
        break;
    case WL_POINTER_AXIS_HORIZONTAL_SCROLL:
        sb_axis = SB_POINTER_SCROLL_AXIS_HORIZONTAL_SCROLL;
        break;
    default:
        break;
    }

    pointer->scroll.frame = false;
    pointer->scroll.axis = sb_axis;
    pointer->scroll.value = val;
}

static void pointer_frame_handler(void *data,
                                  struct wl_pointer *wl_pointer)
{
    sb_pointer_priv_t *pointer = (sb_pointer_priv_t*)data;

    // sb_log_debug("pointer_frame_handler()\n");
    if (pointer->scroll.frame == false && pointer->scroll.value != 0.0f) {
        sb_log_debug(" = Pointer frame for scroll.\n");
        // Post scroll event.
        sb_event_t *event = sb_event_new(SB_EVENT_TARGET_TYPE_VIEW,
            pointer->view, SB_EVENT_TYPE_POINTER_SCROLL);
        event->scroll.axis = pointer->scroll.axis;
        event->scroll.source = pointer->scroll.source;
        event->scroll.value = pointer->scroll.value;

        sb_application_post_event(pointer->sb_application, event);

        // Reset scroll info.
        pointer->scroll.frame = true;
        pointer->scroll.value = 0.0f;
    }
    if (pointer->scroll.ver_stop == true || pointer->scroll.hor_stop == true) {
        sb_log_debug(" == Pointer frame for stop!\n");
        pointer->scroll.ver_stop = false;
        pointer->scroll.hor_stop = false;
    }
    // sb_log_debug("pointer_frame_handler()\n");
}

static void pointer_axis_source_handler(void *data,
                                        struct wl_pointer *wl_pointer,
                                        uint32_t axis_source)
{
    sb_pointer_priv_t *pointer = (sb_pointer_priv_t*)data;

    enum sb_pointer_scroll_source source;
    switch (axis_source) {
    case WL_POINTER_AXIS_SOURCE_WHEEL:
        source = SB_POINTER_SCROLL_SOURCE_WHEEL;
        sb_log_debug("pointer_axis_source_handler() - SB_POINTER_SCROLL_SOURCE_WHEEL\n");
        break;
    case WL_POINTER_AXIS_SOURCE_FINGER:
        source = SB_POINTER_SCROLL_SOURCE_FINGER;
        sb_log_debug("pointer_axis_source_handler() - SB_POINTER_SCROLL_SOURCE_FINGER\n");
        break;
    case WL_POINTER_AXIS_SOURCE_CONTINUOUS:
        source = SB_POINTER_SCROLL_SOURCE_CONTINUOUS;
        sb_log_debug("pointer_axis_source_handler() - SB_POINTER_SCROLL_SOURCE_CONTINUOUS\n");
        break;
    case WL_POINTER_AXIS_SOURCE_WHEEL_TILT:
        source = SB_POINTER_SCROLL_SOURCE_WHEEL_TILT;
        sb_log_debug("pointer_axis_source_handler() - SB_POINTER_SCROLL_SOURCE_WHEEL_TILT\n");
        break;
    default:
        source = SB_POINTER_SCROLL_SOURCE_WHEEL;
        break;
    }

    pointer->scroll.frame = false;
    pointer->scroll.source = source;
}

static void pointer_axis_stop_handler(void *data,
                                      struct wl_pointer *wl_pointer,
                                      uint32_t time,
                                      uint32_t axis)
{
    sb_pointer_priv_t *pointer = (sb_pointer_priv_t*)data;

    enum sb_pointer_scroll_axis sb_axis =
        SB_POINTER_SCROLL_AXIS_VERTICAL_SCROLL;
    switch (axis) {
    case WL_POINTER_AXIS_VERTICAL_SCROLL:
        pointer->scroll.ver_stop = true;
        sb_axis = SB_POINTER_SCROLL_AXIS_VERTICAL_SCROLL;
        break;
    case WL_POINTER_AXIS_HORIZONTAL_SCROLL:
        pointer->scroll.hor_stop = true;
        sb_axis = SB_POINTER_SCROLL_AXIS_HORIZONTAL_SCROLL;
        break;
    default:
        break;
    }

    pointer->scroll.stop_axis = sb_axis;
    pointer->scroll.frame = false;
    sb_log_debug("pointer_axis_stop_handler() - axis: %d\n", axis);

}

static void pointer_axis_discrete_handler(void *data,
                                          struct wl_pointer *wl_pointer,
                                          uint32_t axis,
                                          int32_t discrete)
{
    // Deprecated.
}

static const struct wl_pointer_listener pointer_listener = {
    .enter = pointer_enter_handler,
    .leave = pointer_leave_handler,
    .motion = pointer_motion_handler,
    .button = pointer_button_handler,
    .axis = pointer_axis_handler,
    .frame = pointer_frame_handler,
    .axis_source = pointer_axis_source_handler,
    .axis_stop = pointer_axis_stop_handler,
    .axis_discrete = pointer_axis_discrete_handler, // Deprecated since 8.
};

void sb_pointer_priv_init(sb_pointer_priv_t *pointer)
{
    pointer->sb_application = NULL;

    pointer->wl_pointer = NULL;
    pointer->wp_cursor_shape_manager_v1 = NULL;
    pointer->current_wl_surface = NULL;
    pointer->view = NULL;
    pointer->enter_serial = 0;
    pointer->leave_serial = 0;
    pointer->button_serial = 0;
    pointer->position.x = 0.0f;
    pointer->position.y = 0.0f;
    pointer->cursor = NULL;
    pointer->click.view = NULL;
    pointer->click.button = SB_POINTER_BUTTON_NONE;
    pointer->double_click.view = NULL;
    pointer->double_click.click_count = 0;
    pointer->double_click.time = 0;
    pointer->double_click.button = SB_POINTER_BUTTON_NONE;
    // Init scroll info.
    // pointer->scroll.axis = ??;
    // pointer->scroll.source = ??;
    pointer->scroll.value = 0.0f;
    pointer->scroll.ver_stop = false;
    pointer->scroll.hor_stop = false;
    // pointer->scroll.stop_axis = ??;
    pointer->scroll.frame = false;
}

void sb_pointer_priv_add_listener(
    sb_pointer_priv_t *pointer, struct wl_seat *wl_seat)
{
    pointer->wl_pointer = wl_seat_get_pointer(wl_seat);
    wl_pointer_add_listener(pointer->wl_pointer, &pointer_listener,
        (void*)pointer);
}
