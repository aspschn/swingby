#include <swingby/application.h>

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <linux/input.h>

#include <wayland-client.h>
#include <wayland-protocols/stable/xdg-shell.h>

#include <swingby/log.h>
#include <swingby/surface.h>
#include <swingby/desktop-surface.h>
#include <swingby/view.h>
#include <swingby/list.h>
#include <swingby/input.h>
#include <swingby/cursor.h>
#include <swingby/event.h>
#include <swingby/event-dispatcher.h>

struct sb_application_t {
    /// `struct wl_display`.
    struct wl_display *_wl_display;
    /// `struct wl_registry`.
    struct wl_registry *_wl_registry;
    struct wl_compositor *_wl_compositor;
    struct xdg_wm_base *_xdg_wm_base;
    struct wl_seat *_wl_seat;
    struct wl_pointer *_wl_pointer;
    struct wl_keyboard *_wl_keyboard;
    struct wl_touch *_wl_touch;
    /// \brief Current pointer surface.
    ///
    /// Pointer motion handler not pass `struct wl_surface` object.
    /// Store this information when pointer entered to the surface.
    struct wl_surface *_pointer_surface;
    /// \brief Current pointer view.
    ///
    /// Store the position of the view under the pointer.
    sb_view_t *_pointer_view;
    /// \brief Pointer event position.
    ///
    /// Pointer button and axis event not pass the position.
    /// Store this information when pointer moved.
    sb_point_t _pointer_pos;
    /// \brief Pointer button event serial.
    uint32_t pointer_button_serial;
    /// \brief Pointer enter event information.
    struct {
        uint32_t serial;
    } enter;
    /// \brief Click event information.
    struct {
        sb_view_t *view;
        sb_pointer_button button;
    } click;
    struct {
        sb_view_t *view;
        uint32_t click_count;
        uint32_t time;
        sb_pointer_button button;
    } double_click;
    /// \brief List of the desktop surfaces.
    sb_list_t *_desktop_surfaces;
    /// \brief Default cursor when view not set cursor.
    sb_cursor_t *cursor;
    /// \brief An event dispatcher.
    sb_event_dispatcher_t *_event_dispatcher;
};

// Singleton object.
static sb_application_t *_sb_application_instance = NULL;

//!<============
//!< Registry
//!<============

static void app_global_handler(void *data,
                               struct wl_registry *wl_registry,
                               uint32_t name,
                               const char *interface,
                               uint32_t version);

static void app_global_remove_handler(void *data,
                                      struct wl_registry *wl_registry,
                                      uint32_t name);

static const struct wl_registry_listener app_registry_listener = {
    .global = app_global_handler,
    .global_remove = app_global_remove_handler,
};

//!<=========
//!< XDG
//!<=========

static void xdg_wm_base_ping_handler(void *data,
                                     struct xdg_wm_base *xdg_wm_base,
                                     uint32_t serial);

static const struct xdg_wm_base_listener app_xdg_wm_base_listener = {
    .ping = xdg_wm_base_ping_handler,
};

//!<===========
//!< Output
//!<===========

static void output_geometry_handler(void *data,
                                       struct wl_output *wl_output,
                                       int32_t x,
                                       int32_t y,
                                       int32_t physical_width,
                                       int32_t physical_height,
                                       int32_t subpixel,
                                       const char *make,
                                       const char *model,
                                       int32_t transform);

static void output_mode_handler(void *data,
                                struct wl_output *wl_output,
                                uint32_t flags,
                                int32_t width,
                                int32_t height,
                                int32_t refresh);

static void output_done_handler(void *data,
                                struct wl_output *wl_output);

static void output_scale_handler(void *data,
                                 struct wl_output *wl_output,
                                 int32_t factor);

static void output_name_handler(void *data,
                                struct wl_output *wl_output,
                                const char *name);

static void output_description_handler(void *data,
                                       struct wl_output *wl_output,
                                       const char *description);

static const struct wl_output_listener output_listener = {
    .geometry = output_geometry_handler,
    .mode = output_mode_handler,
    .done = output_done_handler,
    .scale = output_scale_handler,
    .name = output_name_handler,
    .description = output_description_handler,
};

//!<===========
//!< Pointer
//!<===========

static void pointer_enter_handler(void *data,
                                  struct wl_pointer *wl_pointer,
                                  uint32_t serial,
                                  struct wl_surface *surface,
                                  wl_fixed_t sx,
                                  wl_fixed_t sy);

static void pointer_leave_handler(void *data,
                                  struct wl_pointer *pointer,
                                  uint32_t serial,
                                  struct wl_surface *surface);

static void pointer_motion_handler(void *data,
                                   struct wl_pointer *wl_pointer,
                                   uint32_t time,
                                   wl_fixed_t sx,
                                   wl_fixed_t sy);

static void pointer_button_handler(void *data,
                                   struct wl_pointer *pointer,
                                   uint32_t serial,
                                   uint32_t time,
                                   uint32_t button,
                                   uint32_t state);

static void pointer_axis_handler(void *data,
                                 struct wl_pointer *wl_pointer,
                                 uint32_t time,
                                 uint32_t axis,
                                 wl_fixed_t value);

static const struct wl_pointer_listener pointer_listener = {
    .enter = pointer_enter_handler,
    .leave = pointer_leave_handler,
    .motion = pointer_motion_handler,
    .button = pointer_button_handler,
    .axis = pointer_axis_handler,
};

//!<=========
//!< Seat
//!==========

static void seat_capabilities_handler(void *data,
                                      struct wl_seat *wl_seat,
                                      uint32_t capabilities);

static void seat_name_handler(void *data,
                              struct wl_seat *wl_seat,
                              const char *name);

static const struct wl_seat_listener seat_listener = {
    .capabilities = seat_capabilities_handler,
    .name = seat_name_handler,
};

//!<====================
//!< Helper Functions
//!<====================

/// \brief Find matching surface with wl_surface.
static sb_surface_t* _find_surface(sb_application_t *app,
                                   struct wl_surface *wl_surface)
{
    sb_surface_t *found = NULL;
    sb_list_t *list = app->_desktop_surfaces;
    for (int i = 0; i < sb_list_length(list); ++i) {
        sb_desktop_surface_t *desktop_surface = sb_list_at(list, i);
        sb_surface_t *surface = sb_desktop_surface_surface(desktop_surface);
        if (sb_surface_wl_surface(surface) == wl_surface) {
            found = surface;
            break;
        }
    }

    return found;
}

/// \brief Find most child view of the root view.
static sb_view_t* _find_most_child(sb_view_t *view,
                                   sb_point_t *position)
{
    sb_list_t *children = sb_view_children(view);

    if (sb_list_length(children) == 0) {
        return view;
    }
    sb_view_t *child = sb_view_child_at(view, position);

    if (child == NULL) {
        return view;
    }

    position->x = position->x - sb_view_geometry(child)->pos.x;
    position->y = position->y - sb_view_geometry(child)->pos.y;

    return _find_most_child(child, position);
}

/// \brief Linux button to Foundation pointer button.
sb_pointer_button _from_linux_button(uint32_t button)
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

/// \brief What is this function's purpose?
bool _is_child_of(sb_view_t *view, sb_view_t *other)
{
    sb_list_t *children = sb_view_children(view);

    if (sb_list_length(children) == 0) {
        return false;
    }

    for (int i = 0; i < sb_list_length(children); ++i) {
        sb_view_t *child = sb_list_at(children, i);
        bool result = _is_child_of(child, other);
        if (result == true) {
            return true;
        }
    }

    return false;
}

static void _reset_double_click(sb_application_t *application)
{
    application->double_click.view = NULL;
    application->double_click.click_count = 0;
    application->double_click.time = 0;
    application->double_click.button = SB_POINTER_BUTTON_NONE;
}

//!<===============
//!< Application
//!<===============

sb_application_t* sb_application_new(int argc, char *argv[])
{
    sb_application_t *app = malloc(sizeof(sb_application_t));

    app->_wl_display = wl_display_connect(NULL);

    // Null initializations.
    app->_wl_seat = NULL;
    app->_wl_pointer = NULL;
    app->_wl_keyboard = NULL;
    app->_wl_touch = NULL;
    app->_pointer_surface = NULL;
    app->_pointer_view = NULL;
    app->click.view = NULL;
    app->click.button = SB_POINTER_BUTTON_NONE;

    _reset_double_click(app);

    app->_wl_registry = wl_display_get_registry(app->_wl_display);
    wl_registry_add_listener(app->_wl_registry, &app_registry_listener,
        (void*)app);

    wl_display_dispatch(app->_wl_display);
    wl_display_roundtrip(app->_wl_display);

    xdg_wm_base_add_listener(app->_xdg_wm_base, &app_xdg_wm_base_listener,
        NULL);

    // Desktop surface list.
    app->_desktop_surfaces = sb_list_new();

    // Event dispatcher.
    app->_event_dispatcher = sb_event_dispatcher_new();

    app->cursor = NULL;

    _sb_application_instance = app;

    return app;
}

sb_application_t* sb_application_instance()
{
    return _sb_application_instance;
}

uint32_t sb_application_pointer_button_serial(sb_application_t *application)
{
    return application->pointer_button_serial;
}

void sb_application_post_event(sb_application_t *application,
                               sb_event_t *event)
{
    sb_event_dispatcher_post_event(application->_event_dispatcher, event);
}

void sb_application_register_desktop_surface(sb_application_t *application,
    sb_desktop_surface_t *desktop_surface)
{
    sb_list_push(application->_desktop_surfaces, (void*)desktop_surface);
}

void sb_application_unregister_desktop_surface(sb_application_t *application,
    sb_desktop_surface_t *desktop_surface)
{
    // TODO: Implementation.
    sb_list_t *list = application->_desktop_surfaces;
    uint64_t length = sb_list_length(list);
    int64_t index = -1;
    for (uint64_t i = 0; i < length; ++i) {
        if (sb_list_at(list, i) == desktop_surface) {
            index = i;
            break;
        }
    }
    if (index != -1) {
        sb_list_remove(list, index);
    }
}

struct wl_display* sb_application_wl_display(
    sb_application_t *application)
{
    return application->_wl_display;
}

struct wl_compositor* sb_application_wl_compositor(
    sb_application_t *application)
{
    return application->_wl_compositor;
}

struct xdg_wm_base* sb_application_xdg_wm_base(sb_application_t *application)
{
    return application->_xdg_wm_base;
}

struct wl_seat* sb_application_wl_seat(sb_application_t *application)
{
    return application->_wl_seat;
}

int sb_application_exec(sb_application_t *application)
{
    while (wl_display_dispatch(application->_wl_display) != -1) {
        // sb_log_debug("wl_display_dispatch() - desktop surfaces: %d\n",
        //              sb_list_length(application->_desktop_surfaces));
        sb_event_dispatcher_process_events(application->_event_dispatcher);
        // Exit event loop when last desktop surface closed.
        if (sb_list_length(application->_desktop_surfaces) == 0) {
            break;
        }
    }

    return 0;
}


//!<==============
//!< Registry
//!<==============

static void app_global_handler(void *data,
                               struct wl_registry *wl_registry,
                               uint32_t name,
                               const char *interface,
                               uint32_t version)
{
    sb_application_t *app = (sb_application_t*)data;

    if (strcmp(interface, "wl_compositor") == 0) {
        app->_wl_compositor = wl_registry_bind(wl_registry,
            name, &wl_compositor_interface, version);
    } else if (strcmp(interface, "xdg_wm_base") == 0) {
        app->_xdg_wm_base = wl_registry_bind(wl_registry,
            name, &xdg_wm_base_interface, 1);
    } else if (strcmp(interface, "wl_seat") == 0) {
        app->_wl_seat = wl_registry_bind(wl_registry,
            name, &wl_seat_interface, 4);
        wl_seat_add_listener(app->_wl_seat, &seat_listener, (void*)app);
    } else if (strcmp(interface, "wl_output") == 0) {
        sb_log_debug("wl_output - name: %d\n", name);
        struct wl_output *wl_output = wl_registry_bind(wl_registry,
            name, &wl_output_interface, 4);
        wl_output_add_listener(wl_output, &output_listener, (void*)app);
    }
}

static void app_global_remove_handler(void *data,
                                      struct wl_registry *wl_registry,
                                      uint32_t name)
{
    sb_log_debug("global_remove_handler() - name: %d\n", name);
}

//!<===========
//!< XDG
//!<===========

static void xdg_wm_base_ping_handler(void *data,
                                     struct xdg_wm_base *xdg_wm_base,
                                     uint32_t serial)
{
    xdg_wm_base_pong(xdg_wm_base, serial);
}

//!<===========
//!< Output
//!<===========

static void output_geometry_handler(void *data,
                                    struct wl_output *wl_output,
                                    int32_t x,
                                    int32_t y,
                                    int32_t physical_width,
                                    int32_t physical_height,
                                    int32_t subpixel,
                                    const char *make,
                                    const char *model,
                                    int32_t transform)
{
    sb_log_debug("output_geometry_handler() - %p\n", wl_output);
}

static void output_mode_handler(void *data,
                                struct wl_output *wl_output,
                                uint32_t flags,
                                int32_t width,
                                int32_t height,
                                int32_t refresh)
{
    sb_log_debug("output_mode_handler() - %p\n", wl_output);
}

static void output_done_handler(void *data,
                                struct wl_output *wl_output)
{
    sb_log_debug("output_done_handler() - %p\n", wl_output);
}

static void output_scale_handler(void *data,
                                 struct wl_output *wl_output,
                                 int32_t factor)
{
    sb_log_debug("output_scale_handler() - %p\n", wl_output);
}

static void output_name_handler(void *data,
                                struct wl_output *wl_output,
                                const char *name)
{
    sb_log_debug("output_name_handler() - %p\n", wl_output);
}

static void output_description_handler(void *data,
                                       struct wl_output *wl_output,
                                       const char *description)
{
    sb_log_debug("outout_description_handler() - %p\n", wl_output);
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
    sb_application_t *app = (sb_application_t*)data;

    app->_pointer_surface = wl_surface;

    // Set the serial.
    app->enter.serial = serial;

    // TEST cursor.
    // Set default cursor.
    if (app->cursor == NULL) {
        sb_point_t hot_spot;
        hot_spot.x = 0;
        hot_spot.y = 0;
        app->cursor = sb_cursor_new(SB_CURSOR_SHAPE_ARROW, &hot_spot);
    }

    sb_surface_t *cursor_surface = sb_cursor_surface(app->cursor);
    wl_pointer_set_cursor(wl_pointer,
        serial, sb_surface_wl_surface(cursor_surface), 0, 0);

    float x = wl_fixed_to_double(sx);
    float y = wl_fixed_to_double(sy);

    // Find the surface.
    sb_surface_t *found = _find_surface(app, wl_surface);

    // Make an event.
    sb_event_t *event = sb_event_new(SB_EVENT_TARGET_TYPE_SURFACE,
        (void*)found,
        SB_EVENT_TYPE_POINTER_ENTER);
    event->pointer.button = SB_POINTER_BUTTON_NONE;
    event->pointer.position.x = x;
    event->pointer.position.y = y;

    // Post the event.
    sb_application_post_event(app, event);

    // Find most child.
    sb_view_t *root_view = sb_surface_root_view(found);
    sb_point_t position;
    position.x = x;
    position.y = y;
    sb_log_debug(" == root view: %p ==\n", root_view);
    sb_view_t *view = _find_most_child(root_view, &position);

    app->_pointer_view = view;

    // Post the event (view).
    _post_pointer_enter_event(view, position.x, position.y);
}

static void pointer_leave_handler(void *data,
                                  struct wl_pointer *pointer,
                                  uint32_t serial,
                                  struct wl_surface *surface)
{
    //
}

static void pointer_motion_handler(void *data,
                                   struct wl_pointer *wl_pointer,
                                   uint32_t time,
                                   wl_fixed_t sx,
                                   wl_fixed_t sy)
{
    sb_application_t *app = (sb_application_t*)data;

    float x = wl_fixed_to_double(sx);
    float y = wl_fixed_to_double(sy);

    // Store the position.
    app->_pointer_pos.x = x;
    app->_pointer_pos.y = y;

    // Find the surface.
    sb_surface_t *surface = _find_surface(app, app->_pointer_surface);

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

        sb_application_post_event(app, move_event);
    }

    // Check difference.
    if (view != app->_pointer_view) {
        // Post the leave event for the previous view.
        // TODO: Leave position.
        _post_pointer_leave_event(app->_pointer_view, 0.0f, 0.0f);

        app->_pointer_view = view;

        // Post the event.
        _post_pointer_enter_event(view, pos.x, pos.y);
    }
}

static void pointer_button_handler(void *data,
                                   struct wl_pointer *pointer,
                                   uint32_t serial,
                                   uint32_t time,
                                   uint32_t button,
                                   uint32_t state)
{
    sb_application_t *app = (sb_application_t*)data;

    app->pointer_button_serial = serial;

    float x = app->_pointer_pos.x;
    float y = app->_pointer_pos.y;

    // Find the surface.
    sb_surface_t *surface = _find_surface(app, app->_pointer_surface);

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
    sb_application_post_event(app, event);

    // Click event.
    if (state == WL_POINTER_BUTTON_STATE_PRESSED) {
        app->click.view = view;
        app->click.button = _from_linux_button(button);
    } else if (state == WL_POINTER_BUTTON_STATE_RELEASED) {
        if (view == app->click.view) {
            sb_event_t *click_event = sb_event_new(SB_EVENT_TARGET_TYPE_VIEW,
                (void*)view,
                SB_EVENT_TYPE_POINTER_CLICK);
            click_event->pointer.button = app->click.button;
            click_event->pointer.position = pos;

            app->click.view = NULL;

            sb_application_post_event(app, click_event);

            // Double click.
            app->double_click.click_count += 1;

            if (app->double_click.view == NULL) {
                app->double_click.view = view;
                app->double_click.button = _from_linux_button(button);
            }
            // Reset double click info if different view or button.
            if (app->double_click.view != view &&
                app->double_click.button != _from_linux_button(button)) {
                _reset_double_click(app);
            }
            // Store time if click count is 1.
            if (app->double_click.click_count == 1) {
                app->double_click.time = time;
            }
            if (app->double_click.click_count == 2 &&
                app->double_click.view == view) {
                uint32_t diff = time - app->double_click.time;
                if (diff <= 1000) {
                    sb_log_debug("DOUBLE CLICK! %p\n", view);
                    sb_event_t *dbl_click_event = sb_event_new(
                        SB_EVENT_TARGET_TYPE_VIEW,
                        view,
                        SB_EVENT_TYPE_POINTER_DOUBLE_CLICK);
                    sb_application_post_event(app, dbl_click_event);
                }
                _reset_double_click(app);
            }
            if (app->double_click.click_count > 2) {
                _reset_double_click(app);
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
    //
}

//!<=========
//!< Seat
//!<=========

static void seat_capabilities_handler(void *data,
                                      struct wl_seat *wl_seat,
                                      uint32_t capabilities)
{
    sb_application_t *app = (sb_application_t*)data;

    if (capabilities & WL_SEAT_CAPABILITY_POINTER) {
        app->_wl_pointer = wl_seat_get_pointer(wl_seat);
        wl_pointer_add_listener(app->_wl_pointer, &pointer_listener,
            (void*)app);
    }
}

static void seat_name_handler(void *data,
                              struct wl_seat *wl_seat,
                              const char *name)
{
    sb_log_debug("Seat name: %s\n", name);
}
