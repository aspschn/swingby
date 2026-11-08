#include <swingby/toplevel.h>

#include <stdbool.h>
#include <stdlib.h>

#include <wayland-client.h>
#include <wayland-protocols/stable/xdg-shell.h>

#include <swingby/log.h>
#include <swingby/size.h>
#include <swingby/rect.h>
#include <swingby/surface.h>
#include <swingby/application.h>
#include <swingby/list.h>
#include <swingby/event.h>
#include <swingby/surface-role.h>

#include "helpers/shared.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

struct sb_toplevel_t {
    sb_surface_role_header_t role;
    sb_surface_t *surface;
    sb_toplevel_t *parent;
    struct xdg_surface *xdg_surface;
    struct xdg_toplevel *xdg_toplevel;
    sb_rect_i_t wm_geometry;
    sb_size_i_t minimum_size;
    sb_toplevel_state_flags states;
    struct {
        /// \brief Initial resizing configure event gives me the garbage values.
        ///
        /// Ignore first resizing event since it contains garbage values.
        /// The reason why is IDK. However ignore this and it will works anyway.
        bool initial_resizing;
    } toplevel;
    sb_list_t *event_listeners;
};

static void toplevel_on_event(sb_event_t *event, void *user_data)
{
    sb_toplevel_t *toplevel = user_data;

    _event_listener_filter_for_each(toplevel->event_listeners,
        event->type, event);
}

//!<==============
//!< XDG Surface
//!<==============

static void xdg_surface_configure_handler(void *data,
                                          struct xdg_surface *xdg_surface,
                                          uint32_t serial);

static struct xdg_surface_listener xdg_surface_listener = {
    .configure = xdg_surface_configure_handler,
};

//!<===============
//!< XDG Toplevel
//!<===============

static void xdg_toplevel_configure_handler(void *data,
                                           struct xdg_toplevel *xdg_toplevel,
                                           int32_t width,
                                           int32_t height,
                                           struct wl_array *states);

static void xdg_toplevel_close_handler(void *data,
                                       struct xdg_toplevel *xdg_toplevel);

static struct xdg_toplevel_listener xdg_toplevel_listener = {
    .configure = xdg_toplevel_configure_handler,
    .close = xdg_toplevel_close_handler,
};

//!<=====================
//!< Helper Functions
//!<=====================

static sb_event_t* _state_change_event_new(
    sb_toplevel_t *toplevel,
    enum sb_toplevel_state state,
    bool value,
    int32_t width,
    int32_t height)
{
    sb_event_t *event = sb_event_new(SB_EVENT_TARGET_TYPE_DESKTOP_SURFACE,
        toplevel,
        SB_EVENT_TYPE_STATE_CHANGE);
    event->state_change.state = state;
    event->state_change.value = value;
    event->state_change.size.width = width;
    event->state_change.size.height = height;

    return event;
}

static void _set_toplevel_parent(sb_toplevel_t *parent,
                                 sb_toplevel_t *child)
{
    if (parent->xdg_toplevel == NULL) {
        sb_log_warn("Parent must show before child show.\n");
        return;
    }
    xdg_toplevel_set_parent(child->xdg_toplevel, parent->xdg_toplevel);
}


sb_toplevel_t* sb_toplevel_new()
{
    sb_toplevel_t *toplevel = malloc(sizeof(sb_toplevel_t));

    // Initialize the members.
    toplevel->parent = NULL;
    toplevel->xdg_surface = NULL;
    toplevel->xdg_toplevel = NULL;

    toplevel->wm_geometry.position.x = 0;
    toplevel->wm_geometry.position.y = 0;
    toplevel->wm_geometry.size.width = 0;
    toplevel->wm_geometry.size.height = 0;

    toplevel->states = SB_TOPLEVEL_STATE_NORMAL;
    toplevel->toplevel.initial_resizing = true;

    toplevel->event_listeners = sb_list_new();

    // Create a surface.
    toplevel->surface = sb_surface_new();

    // Surface role header.
    toplevel->role.name = "toplevel";
    toplevel->role.surface = toplevel->surface;
    toplevel->role.event_filter = toplevel_on_event;

    // Register this role to application.
    sb_application_register_surface_role(sb_application_instance(),
        &toplevel->role);

    return toplevel;
}

void sb_toplevel_set_parent(sb_toplevel_t *toplevel,
                            sb_toplevel_t *parent)
{
    toplevel->parent = parent;
}

sb_surface_t* sb_toplevel_surface(sb_toplevel_t *toplevel)
{
    return toplevel->surface;
}

void sb_toplevel_show(sb_toplevel_t *toplevel)
{
    // Wayland objects.
    sb_application_t *app = sb_application_instance();
    struct xdg_wm_base *xdg_wm_base = sb_application_xdg_wm_base(app);
    struct wl_surface *wl_surface = sb_surface_wl_surface(
        toplevel->surface);

    // Create XDG surface.
    struct xdg_surface *xdg_surface = xdg_wm_base_get_xdg_surface(xdg_wm_base,
        wl_surface);
    toplevel->xdg_surface = xdg_surface;
    xdg_surface_add_listener(toplevel->xdg_surface,
        &xdg_surface_listener, NULL);

    // Create toplevel or popup.
    if (true) {
        toplevel->xdg_toplevel = xdg_surface_get_toplevel(xdg_surface);
        xdg_toplevel_add_listener(toplevel->xdg_toplevel,
            &xdg_toplevel_listener, (void*)toplevel);

        // Must commit and roundtrip.
        sb_surface_commit(sb_toplevel_surface(toplevel));
        wl_display_roundtrip(sb_application_wl_display(app));

        // Set minimum size.
        sb_size_i_t min_size;
        min_size.width = 100;
        min_size.height = 100;
        sb_toplevel_set_minimum_size(toplevel, min_size);
    }

    // Commit.
    if (true) {
        sb_surface_attach(toplevel->surface);

        sb_surface_update(toplevel->surface);

        // Set parent toplevel.
        if (toplevel->parent != NULL) {
            _set_toplevel_parent(toplevel->parent, toplevel);
        }
    }

    sb_event_t *event = sb_event_new(SB_EVENT_TARGET_TYPE_DESKTOP_SURFACE,
        toplevel, SB_EVENT_TYPE_SHOW);
    sb_application_post_event(sb_application_instance(), event);

    // TEST
    // sb_surface_on_request_update(desktop_surface->_surface);
}

void sb_toplevel_hide(sb_toplevel_t *toplevel)
{
    if (true) {
        xdg_toplevel_destroy(toplevel->xdg_toplevel);
        toplevel->xdg_toplevel = NULL;
    }

    xdg_surface_destroy(toplevel->xdg_surface);
    toplevel->xdg_surface = NULL;

    sb_surface_detach(toplevel->surface);

    sb_event_t *event = sb_event_new(SB_EVENT_TARGET_TYPE_DESKTOP_SURFACE,
        toplevel, SB_EVENT_TYPE_HIDE);
    sb_application_post_event(sb_application_instance(), event);
}

sb_toplevel_state_flags sb_toplevel_states(sb_toplevel_t *toplevel)
{
    return toplevel->states;
}

void sb_toplevel_set_wm_geometry(sb_toplevel_t *toplevel, sb_rect_i_t geometry)
{
    if (geometry.size.width <= 0 || geometry.size.height <= 0) {
        sb_log_warn("sb_desktop_surface_set_wm_geometry - "
            "values can't be zero or negative: %dx%d\n",
            geometry.size.width, geometry.size.height);
        return;
    }
    toplevel->wm_geometry = geometry;

    if (toplevel->xdg_surface == NULL) {
        sb_log_warn("XDG surface of the desktop surface is NULL!\n");
    }

    xdg_surface_set_window_geometry(toplevel->xdg_surface,
        geometry.position.x, geometry.position.y,
        geometry.size.width, geometry.size.height);
}

sb_size_i_t sb_toplevel_minimum_size(sb_toplevel_t *toplevel)
{
    return toplevel->minimum_size;
}

void sb_toplevel_set_minimum_size(sb_toplevel_t *toplevel, sb_size_i_t size)
{
    toplevel->minimum_size = size;

    xdg_toplevel_set_min_size(toplevel->xdg_toplevel,
        size.width, size.height);
}

void sb_toplevel_close(sb_toplevel_t *toplevel)
{
    // TODO: Free memory.

    sb_application_t *app = sb_application_instance();

    sb_application_unregister_surface_role(app, &toplevel->role);
}

void sb_toplevel_move(sb_toplevel_t *toplevel)
{
    sb_application_t *app = sb_application_instance();

    xdg_toplevel_move(toplevel->xdg_toplevel,
        sb_application_wl_seat(app),
        sb_application_pointer_button_serial(app));
}

void sb_toplevel_resize(sb_toplevel_t *toplevel,
    enum sb_toplevel_resize_edge edge)
{
    sb_application_t *app = sb_application_instance();

    enum xdg_toplevel_resize_edge resize_edge = 0;
    switch (edge) {
    case SB_TOPLEVEL_RESIZE_EDGE_NONE:
        resize_edge = XDG_TOPLEVEL_RESIZE_EDGE_NONE;
        break;
    case SB_TOPLEVEL_RESIZE_EDGE_TOP:
        resize_edge = XDG_TOPLEVEL_RESIZE_EDGE_TOP;
        break;
    case SB_TOPLEVEL_RESIZE_EDGE_BOTTOM:
        resize_edge = XDG_TOPLEVEL_RESIZE_EDGE_BOTTOM;
        break;
    case SB_TOPLEVEL_RESIZE_EDGE_LEFT:
        resize_edge = XDG_TOPLEVEL_RESIZE_EDGE_LEFT;
        break;
    case SB_TOPLEVEL_RESIZE_EDGE_TOP_LEFT:
        resize_edge = XDG_TOPLEVEL_RESIZE_EDGE_TOP_LEFT;
        break;
    case SB_TOPLEVEL_RESIZE_EDGE_BOTTOM_LEFT:
        resize_edge = XDG_TOPLEVEL_RESIZE_EDGE_BOTTOM_LEFT;
        break;
    case SB_TOPLEVEL_RESIZE_EDGE_RIGHT:
        resize_edge = XDG_TOPLEVEL_RESIZE_EDGE_RIGHT;
        break;
    case SB_TOPLEVEL_RESIZE_EDGE_TOP_RIGHT:
        resize_edge = XDG_TOPLEVEL_RESIZE_EDGE_TOP_RIGHT;
        break;
    case SB_TOPLEVEL_RESIZE_EDGE_BOTTOM_RIGHT:
        resize_edge = XDG_TOPLEVEL_RESIZE_EDGE_BOTTOM_RIGHT;
        break;
    default:
        resize_edge = XDG_TOPLEVEL_RESIZE_EDGE_NONE;
        break;
    }

    xdg_toplevel_resize(toplevel->xdg_toplevel,
        sb_application_wl_seat(app),
        sb_application_pointer_button_serial(app),
        resize_edge);
}

void sb_toplevel_set_maximized(sb_toplevel_t *toplevel)
{
    if (toplevel->xdg_toplevel == NULL) {
        sb_log_warn("XDG toplevel of the desktop surface is NULL!\n");
    }
    xdg_toplevel_set_maximized(toplevel->xdg_toplevel);
}

void sb_toplevel_unset_maximized(sb_toplevel_t *toplevel)
{
    if (toplevel->xdg_toplevel == NULL) {
        sb_log_warn("XDG toplevel of the desktop surface is NULL!\n");
    }
    xdg_toplevel_unset_maximized(toplevel->xdg_toplevel);
}

void sb_toplevel_set_minimized(
    sb_toplevel_t *toplevel)
{
    if (toplevel->xdg_toplevel == NULL) {
        sb_log_warn("XDG toplevel of the desktop surface is NULL!\n");
        return;
    }
    xdg_toplevel_set_minimized(toplevel->xdg_toplevel);
}

void sb_toplevel_set_title(sb_toplevel_t *toplevel, const char *title)
{
    xdg_toplevel_set_title(toplevel->xdg_toplevel, title);
}

void sb_toplevel_set_app_id(sb_toplevel_t *toplevel, const char *app_id)
{
    if (toplevel->xdg_toplevel == NULL) {
        sb_log_warn("sb_desktop_surface_toplevel_set_app_id()"
                    " - XDG toplevel is NULL!\n");
        return;
    }
    xdg_toplevel_set_app_id(toplevel->xdg_toplevel, app_id);
}

void sb_toplevel_show_window_menu(sb_toplevel_t *toplevel,
                                  sb_point_i_t position)
{
    if (toplevel->xdg_toplevel == NULL) {
        sb_log_warn("sb_desktop_surface_toplevel_show_window_menu()"
                    " - XDG toplevel is NULL!\n");
        return;
    }

    sb_application_t *app = sb_application_instance();
    xdg_toplevel_show_window_menu(toplevel->xdg_toplevel,
        sb_application_wl_seat(app),
        sb_application_pointer_button_serial(app),
        position.x, position.y
    );
}

struct xdg_surface* sb_toplevel_xdg_surface(const sb_toplevel_t *toplevel)
{
    return toplevel->xdg_surface;
}

void sb_toplevel_free(sb_toplevel_t *toplevel)
{
    sb_surface_free(toplevel->surface);

    free(toplevel);
}

void sb_toplevel_add_event_listener(
    sb_toplevel_t *toplevel,
    enum sb_event_type event_type,
    sb_event_listener_t listener,
    void *user_data)
{
    sb_event_listener_tuple_t *tuple = sb_event_listener_tuple_new(
        event_type, listener, user_data);
    sb_list_push(toplevel->event_listeners, (void*)tuple);
}

//!<===================
//!< XDG Surface
//!<===================

static void xdg_surface_configure_handler(void *data,
                                          struct xdg_surface *xdg_surface,
                                          uint32_t serial)
{
    xdg_surface_ack_configure(xdg_surface, serial);
}

//!<===================
//!< XDG Toplevel
//!<===================

static void xdg_toplevel_configure_handler(void *data,
                                           struct xdg_toplevel *xdg_toplevel,
                                           int32_t width,
                                           int32_t height,
                                           struct wl_array *states)
{
    sb_toplevel_t *toplevel = (sb_toplevel_t*)data;
    // sb_application_t *app = sb_application_instance();

    // XDG configure event does not contain restore states.
    // Manually store the states as boolean flags and compare these at the
    // end of iteration.
    bool maximized = false;
    bool fullscreen = false;
    sb_toplevel_state_flags curr_states = 0;

    enum xdg_toplevel_state *it;
    wl_array_for_each(it, states) {
        enum xdg_toplevel_state state = *(enum xdg_toplevel_state*)it;
        sb_log_debug("wl_array_for_each() - state: %d, %dx%d\n", state, width, height);
        switch (state) {
        case XDG_TOPLEVEL_STATE_MAXIMIZED:
        {
            maximized = true;
            curr_states |= SB_TOPLEVEL_STATE_MAXIMIZED;

            break;
        }
        case XDG_TOPLEVEL_STATE_FULLSCREEN:
            curr_states |= SB_TOPLEVEL_STATE_FULLSCREEN;

            break;
        case XDG_TOPLEVEL_STATE_ACTIVATED:
            curr_states |= SB_TOPLEVEL_STATE_ACTIVATED;

            break;
        case XDG_TOPLEVEL_STATE_RESIZING:
        {
            sb_log_debug("Resize %dx%d\n", width, height);
            curr_states |= SB_TOPLEVEL_STATE_RESIZING;
            if (toplevel->toplevel.initial_resizing == false) {
                sb_surface_t *surface = toplevel->surface;

                sb_size_t old_size = {
                    .width = toplevel->wm_geometry.size.width,
                    .height = toplevel->wm_geometry.size.height,
                };

                sb_size_t new_size;
                new_size.width = width;
                new_size.height = height;

                // Just send a resize event (to the desktop surface).
                sb_event_t *event = sb_event_new(
                    SB_EVENT_TARGET_TYPE_SURFACE_ROLE,
                    toplevel,
                    SB_EVENT_TYPE_RESIZE_REQUEST);
                event->resize.old_size = old_size;
                event->resize.size = new_size;

                sb_application_post_event(sb_application_instance(), event);
            } else {
                // Ignore and set the initial is false.
                toplevel->toplevel.initial_resizing = false;
            }

            break;
        }
        default:
            break;
        }
    }

    // Compare states.
    {
        sb_toplevel_state_flags states;
        states = toplevel->states;

        // Maximized.
        if (!(states & SB_TOPLEVEL_STATE_MAXIMIZED) &&
            curr_states & SB_TOPLEVEL_STATE_MAXIMIZED) {
            toplevel->states |= SB_TOPLEVEL_STATE_MAXIMIZED;

            sb_event_t *event = _state_change_event_new(toplevel,
                SB_TOPLEVEL_STATE_MAXIMIZED, true,
                width, height);

            sb_application_post_event(sb_application_instance(), event);
        } else if (states & SB_TOPLEVEL_STATE_MAXIMIZED &&
            !(curr_states & SB_TOPLEVEL_STATE_MAXIMIZED)) {
            toplevel->states &= ~SB_TOPLEVEL_STATE_MAXIMIZED;

            sb_event_t *event = _state_change_event_new(toplevel,
                SB_TOPLEVEL_STATE_MAXIMIZED, false,
                width, height);

            sb_application_post_event(sb_application_instance(), event);
        }

        // Activated.
        if (!(states & SB_TOPLEVEL_STATE_ACTIVATED) &&
            curr_states & SB_TOPLEVEL_STATE_ACTIVATED) {
            toplevel->states |= SB_TOPLEVEL_STATE_ACTIVATED;

            sb_event_t *event = _state_change_event_new(toplevel,
                SB_TOPLEVEL_STATE_ACTIVATED, true, 0, 0);

            sb_application_post_event(sb_application_instance(), event);
        } else if (states & SB_TOPLEVEL_STATE_ACTIVATED &&
            !(curr_states & SB_TOPLEVEL_STATE_ACTIVATED)) {
            toplevel->states &= ~SB_TOPLEVEL_STATE_ACTIVATED;

            sb_event_t *event = _state_change_event_new(toplevel,
                SB_TOPLEVEL_STATE_ACTIVATED, false, 0, 0);

            sb_application_post_event(sb_application_instance(), event);
        }
    }
}

static void xdg_toplevel_close_handler(void *data,
                                       struct xdg_toplevel *xdg_toplevel)
{
    sb_toplevel_t *toplevel = (sb_toplevel_t*)data;

    sb_toplevel_close(toplevel);
}

#ifdef __cplusplus
}
#endif // __cplusplus
