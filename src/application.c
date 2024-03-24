#include <foundation/application.h>

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <linux/input.h>

#include <wayland-client.h>
#include <wayland-protocols/stable/xdg-shell.h>

#include <foundation/log.h>
#include <foundation/surface.h>
#include <foundation/desktop-surface.h>
#include <foundation/view.h>
#include <foundation/list.h>
#include <foundation/input.h>
#include <foundation/cursor.h>
#include <foundation/event.h>
#include <foundation/event-dispatcher.h>

struct ft_application_t {
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
    ft_view_t *_pointer_view;
    /// \brief Pointer event position.
    ///
    /// Pointer button and axis event not pass the position.
    /// Store this information when pointer moved.
    ft_point_t _pointer_pos;
    /// \brief Pointer button event serial.
    uint32_t pointer_button_serial;
    /// \brief Pointer enter event information.
    struct {
        uint32_t serial;
    } enter;
    /// \brief Click event information.
    struct {
        ft_view_t *view;
        ft_pointer_button button;
    } click;
    struct {
        ft_view_t *view;
        uint32_t click_count;
        uint32_t time;
        ft_pointer_button button;
    } double_click;
    /// \brief List of the desktop surfaces.
    ft_list_t *_desktop_surfaces;
    /// \brief Default cursor when view not set cursor.
    ft_cursor_t *cursor;
    /// \brief An event dispatcher.
    ft_event_dispatcher_t *_event_dispatcher;
};

// Singleton object.
static ft_application_t *_ft_application_instance = NULL;

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
static ft_surface_t* _find_surface(ft_application_t *app,
                                   struct wl_surface *wl_surface)
{
    ft_surface_t *found = NULL;
    ft_list_t *list = app->_desktop_surfaces;
    for (int i = 0; i < ft_list_length(list); ++i) {
        ft_desktop_surface_t *desktop_surface = ft_list_at(list, i);
        ft_surface_t *surface = ft_desktop_surface_surface(desktop_surface);
        if (ft_surface_wl_surface(surface) == wl_surface) {
            found = surface;
            break;
        }
    }

    return found;
}

/// \brief Find most child view of the root view.
static ft_view_t* _find_most_child(ft_view_t *view,
                                   ft_point_t *position)
{
    ft_list_t *children = ft_view_children(view);

    if (ft_list_length(children) == 0) {
        return view;
    }
    ft_view_t *child = ft_view_child_at(view, position);

    if (child == NULL) {
        return view;
    }

    position->x = position->x - ft_view_geometry(child)->pos.x;
    position->y = position->y - ft_view_geometry(child)->pos.y;

    return _find_most_child(child, position);
}

/// \brief Linux button to Foundation pointer button.
ft_pointer_button _from_linux_button(uint32_t button)
{
    switch (button) {
    case BTN_LEFT:
        return FT_POINTER_BUTTON_LEFT;
    case BTN_RIGHT:
        return FT_POINTER_BUTTON_RIGHT;
    case BTN_MIDDLE:
        return FT_POINTER_BUTTON_MIDDLE;
    default:
        return FT_POINTER_BUTTON_UNIMPLEMENTED;
    }
}

/// \brief What is this function's purpose?
bool _is_child_of(ft_view_t *view, ft_view_t *other)
{
    ft_list_t *children = ft_view_children(view);

    if (ft_list_length(children) == 0) {
        return false;
    }

    for (int i = 0; i < ft_list_length(children); ++i) {
        ft_view_t *child = ft_list_at(children, i);
        bool result = _is_child_of(child, other);
        if (result == true) {
            return true;
        }
    }

    return false;
}

static void _reset_double_click(ft_application_t *application)
{
    application->double_click.view = NULL;
    application->double_click.click_count = 0;
    application->double_click.time = 0;
    application->double_click.button = FT_POINTER_BUTTON_NONE;
}

//!<===============
//!< Application
//!<===============

ft_application_t* ft_application_new(int argc, char *argv[])
{
    ft_application_t *app = malloc(sizeof(ft_application_t));

    app->_wl_display = wl_display_connect(NULL);

    // Null initializations.
    app->_wl_seat = NULL;
    app->_wl_pointer = NULL;
    app->_wl_keyboard = NULL;
    app->_wl_touch = NULL;
    app->_pointer_surface = NULL;
    app->_pointer_view = NULL;
    app->click.view = NULL;
    app->click.button = FT_POINTER_BUTTON_NONE;

    _reset_double_click(app);

    app->_wl_registry = wl_display_get_registry(app->_wl_display);
    wl_registry_add_listener(app->_wl_registry, &app_registry_listener,
        (void*)app);

    wl_display_dispatch(app->_wl_display);
    wl_display_roundtrip(app->_wl_display);

    xdg_wm_base_add_listener(app->_xdg_wm_base, &app_xdg_wm_base_listener,
        NULL);

    // Desktop surface list.
    app->_desktop_surfaces = ft_list_new();

    // Event dispatcher.
    app->_event_dispatcher = ft_event_dispatcher_new();

    app->cursor = NULL;

    _ft_application_instance = app;

    return app;
}

ft_application_t* ft_application_instance()
{
    return _ft_application_instance;
}

uint32_t ft_application_pointer_button_serial(ft_application_t *application)
{
    return application->pointer_button_serial;
}

void ft_application_post_event(ft_application_t *application,
                               ft_event_t *event)
{
    ft_event_dispatcher_post_event(application->_event_dispatcher, event);
}

void ft_application_register_desktop_surface(ft_application_t *application,
    ft_desktop_surface_t *desktop_surface)
{
    ft_list_push(application->_desktop_surfaces, (void*)desktop_surface);
}

void ft_application_unregister_desktop_surface(ft_application_t *application,
    ft_desktop_surface_t *desktop_surface)
{
    // TODO: Implementation.
    ft_list_t *list = application->_desktop_surfaces;
    uint64_t length = ft_list_length(list);
    int64_t index = -1;
    for (uint64_t i = 0; i < length; ++i) {
        if (ft_list_at(list, i) == desktop_surface) {
            index = i;
            break;
        }
    }
    if (index != -1) {
        ft_list_remove(list, index);
    }
}

struct wl_display* ft_application_wl_display(
    ft_application_t *application)
{
    return application->_wl_display;
}

struct wl_compositor* ft_application_wl_compositor(
    ft_application_t *application)
{
    return application->_wl_compositor;
}

struct xdg_wm_base* ft_application_xdg_wm_base(ft_application_t *application)
{
    return application->_xdg_wm_base;
}

struct wl_seat* ft_application_wl_seat(ft_application_t *application)
{
    return application->_wl_seat;
}

int ft_application_exec(ft_application_t *application)
{
    while (wl_display_dispatch(application->_wl_display) != -1) {
        // ft_log_debug("wl_display_dispatch() - desktop surfaces: %d\n",
        //              ft_list_length(application->_desktop_surfaces));
        ft_event_dispatcher_process_events(application->_event_dispatcher);
        // Exit event loop when last desktop surface closed.
        if (ft_list_length(application->_desktop_surfaces) == 0) {
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
    ft_application_t *app = (ft_application_t*)data;

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
    }
}

static void app_global_remove_handler(void *data,
                                      struct wl_registry *wl_registry,
                                      uint32_t name)
{
    //
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
    ft_application_t *app = (ft_application_t*)data;

    app->_pointer_surface = wl_surface;

    // Set the serial.
    app->enter.serial = serial;

    // TEST cursor.
    // Set default cursor.
    if (app->cursor == NULL) {
        ft_point_t hot_spot;
        hot_spot.x = 0;
        hot_spot.y = 0;
        app->cursor = ft_cursor_new(FT_CURSOR_SHAPE_ARROW, &hot_spot);

        ft_surface_t *cursor_surface = ft_cursor_surface(app->cursor);
        wl_pointer_set_cursor(wl_pointer,
            serial, ft_surface_wl_surface(cursor_surface), 0, 0);
    }

    float x = wl_fixed_to_double(sx);
    float y = wl_fixed_to_double(sy);

    // Find the surface.
    ft_surface_t *found = _find_surface(app, wl_surface);

    // Make an event.
    ft_event_t *event = ft_event_new(FT_EVENT_TARGET_TYPE_SURFACE,
        (void*)found,
        FT_EVENT_TYPE_POINTER_ENTER);
    event->pointer.button = FT_POINTER_BUTTON_NONE;
    event->pointer.position.x = x;
    event->pointer.position.y = y;

    // Post the event.
    ft_application_post_event(app, event);

    // Find most child.
    ft_view_t *root_view = ft_surface_root_view(found);
    ft_point_t position;
    position.x = x;
    position.y = y;
    ft_log_debug(" == root view: %p ==\n", root_view);
    ft_view_t *view = _find_most_child(root_view, &position);

    app->_pointer_view = view;

    ft_event_t *view_event = ft_event_new(FT_EVENT_TARGET_TYPE_VIEW,
        (void*)view,
        FT_EVENT_TYPE_POINTER_ENTER);
    view_event->pointer.button = FT_POINTER_BUTTON_NONE;
    view_event->pointer.position.x = position.x;
    view_event->pointer.position.y = position.y;

    // Post the event (view).
    ft_application_post_event(app, view_event);
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
    ft_application_t *app = (ft_application_t*)data;

    float x = wl_fixed_to_double(sx);
    float y = wl_fixed_to_double(sy);

    // Store the position.
    app->_pointer_pos.x = x;
    app->_pointer_pos.y = y;

    // Find the surface.
    ft_surface_t *surface = _find_surface(app, app->_pointer_surface);

    // Find most child view.
    ft_point_t pos;
    pos.x = x;
    pos.y = y;
    ft_view_t *view = _find_most_child(ft_surface_root_view(surface), &pos);

    // Pointer move event.
    {
        ft_event_t *move_event = ft_event_new(FT_EVENT_TARGET_TYPE_VIEW,
            (void*)view,
            FT_EVENT_TYPE_POINTER_MOVE);
        move_event->pointer.button = FT_POINTER_BUTTON_NONE;
        move_event->pointer.position.x = pos.x;
        move_event->pointer.position.y = pos.y;

        ft_application_post_event(app, move_event);
    }

    // Check difference.
    if (view != app->_pointer_view) {
        ft_event_t *enter_event = ft_event_new(FT_EVENT_TARGET_TYPE_VIEW,
            (void*)view,
            FT_EVENT_TYPE_POINTER_ENTER);
        enter_event->pointer.button = FT_POINTER_BUTTON_NONE;
        enter_event->pointer.position.x = pos.x;
        enter_event->pointer.position.y = pos.y;

        app->_pointer_view = view;

        // Post the event.
        ft_application_post_event(app, enter_event);
    }
}

static void pointer_button_handler(void *data,
                                   struct wl_pointer *pointer,
                                   uint32_t serial,
                                   uint32_t time,
                                   uint32_t button,
                                   uint32_t state)
{
    ft_application_t *app = (ft_application_t*)data;

    app->pointer_button_serial = serial;

    float x = app->_pointer_pos.x;
    float y = app->_pointer_pos.y;

    // Find the surface.
    ft_surface_t *surface = _find_surface(app, app->_pointer_surface);

    // Find most child view.
    ft_point_t pos = { .x = x, .y = y };
    ft_view_t *view = _find_most_child(ft_surface_root_view(surface), &pos);

    // Set the event type.
    enum ft_event_type evt_type = FT_EVENT_TYPE_POINTER_PRESS;
    if (state == WL_POINTER_BUTTON_STATE_PRESSED) {
        evt_type = FT_EVENT_TYPE_POINTER_PRESS;
    } else if (state == WL_POINTER_BUTTON_STATE_RELEASED) {
        evt_type = FT_EVENT_TYPE_POINTER_RELEASE;
    }
    ft_event_t *event = ft_event_new(FT_EVENT_TARGET_TYPE_VIEW,
        (void*)view,
        evt_type);

    event->pointer.button = _from_linux_button(button);
    event->pointer.position = pos;

    // Post the event.
    ft_application_post_event(app, event);

    // Click event.
    if (state == WL_POINTER_BUTTON_STATE_PRESSED) {
        app->click.view = view;
        app->click.button = _from_linux_button(button);
    } else if (state == WL_POINTER_BUTTON_STATE_RELEASED) {
        if (view == app->click.view) {
            ft_event_t *click_event = ft_event_new(FT_EVENT_TARGET_TYPE_VIEW,
                (void*)view,
                FT_EVENT_TYPE_POINTER_CLICK);
            click_event->pointer.button = app->click.button;
            click_event->pointer.position = pos;

            app->click.view = NULL;

            ft_application_post_event(app, click_event);

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
                    ft_log_debug("DOUBLE CLICK! %p\n", view);
                    ft_event_t *dbl_click_event = ft_event_new(
                        FT_EVENT_TARGET_TYPE_VIEW,
                        view,
                        FT_EVENT_TYPE_POINTER_DOUBLE_CLICK);
                    ft_application_post_event(app, dbl_click_event);
                }
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
    ft_application_t *app = (ft_application_t*)data;

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
    ft_log_debug("Seat name: %s\n", name);
}
