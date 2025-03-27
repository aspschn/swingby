#include <swingby/desktop-surface.h>

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

struct sb_desktop_surface_t {
    sb_surface_t *_surface;
    sb_desktop_surface_role _role;
    sb_desktop_surface_t *parent;
    struct xdg_surface *_xdg_surface;
    struct xdg_toplevel *_xdg_toplevel;
    struct xdg_popup *xdg_popup;
    sb_rect_t wm_geometry;
    struct {
        sb_size_t minimum_size;
        sb_desktop_surface_toplevel_state_flags states;
        /// \brief Initial resizing configure event gives me the garbage values.
        ///
        /// Ignore first resizing event since it contains garbage values.
        /// The reason why is IDK. However ignore this and it will works anyway.
        bool initial_resizing;
    } toplevel;
    sb_list_t *event_listeners;
};

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

//!<================
//!< XDG Popup
//!<================

static void xdg_popup_configure_handler(void *data,
                                        struct xdg_popup *xdg_popup,
                                        int32_t x,
                                        int32_t y,
                                        int32_t width,
                                        int32_t height);

static void xdg_popup_popup_done_handler(void *data,
                                         struct xdg_popup *xdg_popup);

static void xdg_popup_repositioned_handler(void *data,
                                           struct xdg_popup *xdg_popup,
                                           uint32_t token);

static struct xdg_popup_listener xdg_popup_listener = {
    .configure = xdg_popup_configure_handler,
    .popup_done = xdg_popup_popup_done_handler,
    .repositioned = xdg_popup_repositioned_handler,
};

//!<=====================
//!< Helper Functions
//!<=====================

static sb_event_t* _state_change_event_new(
    sb_desktop_surface_t *desktop_surface,
    enum sb_desktop_surface_toplevel_state state,
    bool value,
    int32_t width,
    int32_t height)
{
    sb_event_t *event = sb_event_new(SB_EVENT_TARGET_TYPE_DESKTOP_SURFACE,
        desktop_surface,
        SB_EVENT_TYPE_STATE_CHANGE);
    event->state_change.state = state;
    event->state_change.value = value;
    event->state_change.size.width = width;
    event->state_change.size.height = height;

    return event;
}

static void _set_toplevel_parent(sb_desktop_surface_t *parent,
                                 sb_desktop_surface_t *child)
{
    if (parent->_xdg_toplevel == NULL) {
        sb_log_warn("Parent must show before child show.\n");
        return;
    }
    xdg_toplevel_set_parent(child->_xdg_toplevel, parent->_xdg_toplevel);
}

static void _event_listener_filter_for_each(sb_list_t *listeners,
                                            enum sb_event_type type,
                                            sb_event_t *event)
{
    uint64_t length = sb_list_length(listeners);
    for (uint64_t i = 0; i < length; ++i) {
        sb_event_listener_tuple_t *tuple = sb_list_at(listeners, i);
        if (tuple->type == type) {
            tuple->listener(event);
        }
    }
}


sb_desktop_surface_t* sb_desktop_surface_new(sb_desktop_surface_role role)
{
    sb_desktop_surface_t *d_surface = malloc(sizeof(sb_desktop_surface_t));

    // Initialize the members.
    d_surface->_role = role;
    d_surface->parent = NULL;
    d_surface->_xdg_surface = NULL;
    d_surface->_xdg_toplevel = NULL;
    d_surface->xdg_popup = NULL;

    d_surface->wm_geometry.pos.x = 0;
    d_surface->wm_geometry.pos.y = 0;
    d_surface->wm_geometry.size.width = 0;
    d_surface->wm_geometry.size.height = 0;

    d_surface->toplevel.states = SB_DESKTOP_SURFACE_TOPLEVEL_STATE_NORMAL;
    d_surface->toplevel.initial_resizing = true;

    d_surface->event_listeners = sb_list_new();

    // Create a surface.
    d_surface->_surface = sb_surface_new();

    sb_application_register_desktop_surface(sb_application_instance(),
        d_surface);

    return d_surface;
}

void sb_desktop_surface_set_parent(sb_desktop_surface_t *desktop_surface,
                                   sb_desktop_surface_t *parent)
{
    desktop_surface->parent = parent;
}

sb_surface_t*
sb_desktop_surface_surface(sb_desktop_surface_t *desktop_surface)
{
    return desktop_surface->_surface;
}

void sb_desktop_surface_show(sb_desktop_surface_t *desktop_surface)
{
    // Wayland objects.
    sb_application_t *app = sb_application_instance();
    struct xdg_wm_base *xdg_wm_base = sb_application_xdg_wm_base(app);
    struct wl_surface *wl_surface = sb_surface_wl_surface(
        desktop_surface->_surface);

    // Create XDG surface.
    struct xdg_surface *xdg_surface = xdg_wm_base_get_xdg_surface(xdg_wm_base,
        wl_surface);
    desktop_surface->_xdg_surface = xdg_surface;
    xdg_surface_add_listener(desktop_surface->_xdg_surface,
        &xdg_surface_listener, NULL);

    // Create toplevel or popup.
    if (desktop_surface->_role == SB_DESKTOP_SURFACE_ROLE_TOPLEVEL) {
        desktop_surface->_xdg_toplevel = xdg_surface_get_toplevel(xdg_surface);
        xdg_toplevel_add_listener(desktop_surface->_xdg_toplevel,
            &xdg_toplevel_listener, (void*)desktop_surface);

        // Set parent toplevel.
        if (desktop_surface->parent != NULL) {
            _set_toplevel_parent(desktop_surface->parent, desktop_surface);
        }

        // Must commit and roundtrip.
        sb_surface_commit(sb_desktop_surface_surface(desktop_surface));
        wl_display_roundtrip(sb_application_wl_display(app));

        // Set minimum size.
        sb_size_t min_size;
        min_size.width = 100;
        min_size.height = 100;
        sb_desktop_surface_toplevel_set_minimum_size(desktop_surface,
            &min_size);
    } else if (desktop_surface->_role == SB_DESKTOP_SURFACE_ROLE_POPUP) {
        struct xdg_positioner *positioner = xdg_wm_base_create_positioner(
            xdg_wm_base);
        const sb_size_t *surface_size = sb_surface_size(
            desktop_surface->_surface);
        const sb_size_t *parent_size = sb_surface_size(
            desktop_surface->parent->_surface);
        xdg_positioner_set_size(positioner,
            surface_size->width, surface_size->height);
        xdg_positioner_set_anchor(positioner, XDG_POSITIONER_ANCHOR_BOTTOM);
        xdg_positioner_set_anchor_rect(positioner, 0, 0,
            parent_size->width, parent_size->height);
        xdg_positioner_set_gravity(positioner, XDG_POSITIONER_GRAVITY_BOTTOM);

        desktop_surface->xdg_popup = xdg_surface_get_popup(xdg_surface,
            desktop_surface->parent->_xdg_surface, positioner);

        xdg_popup_add_listener(desktop_surface->xdg_popup, &xdg_popup_listener,
            (void*)desktop_surface);

        xdg_positioner_destroy(positioner);
    }

    // Commit.
    if (desktop_surface->_role == SB_DESKTOP_SURFACE_ROLE_TOPLEVEL) {
        wl_surface_commit(wl_surface);

        sb_surface_attach(desktop_surface->_surface);

        sb_surface_update(desktop_surface->_surface);
    }

    sb_event_t *event = sb_event_new(SB_EVENT_TARGET_TYPE_DESKTOP_SURFACE,
        desktop_surface, SB_EVENT_TYPE_SHOW);
    sb_application_post_event(sb_application_instance(), event);

    // TEST
    // sb_surface_on_request_update(desktop_surface->_surface);
}

void sb_desktop_surface_hide(sb_desktop_surface_t *desktop_surface)
{
    if (desktop_surface->_role == SB_DESKTOP_SURFACE_ROLE_TOPLEVEL) {
        xdg_toplevel_destroy(desktop_surface->_xdg_toplevel);
        desktop_surface->_xdg_toplevel = NULL;
    } else if (desktop_surface->_role == SB_DESKTOP_SURFACE_ROLE_POPUP) {
        xdg_popup_destroy(desktop_surface->xdg_popup);
        desktop_surface->xdg_popup = NULL;

        sb_application_unregister_desktop_surface(sb_application_instance(),
            desktop_surface);
    }

    xdg_surface_destroy(desktop_surface->_xdg_surface);
    desktop_surface->_xdg_surface = NULL;

    sb_surface_detach(desktop_surface->_surface);

    sb_event_t *event = sb_event_new(SB_EVENT_TARGET_TYPE_DESKTOP_SURFACE,
        desktop_surface, SB_EVENT_TYPE_HIDE);
    sb_application_post_event(sb_application_instance(), event);
}

sb_desktop_surface_toplevel_state_flags
sb_desktop_surface_toplevel_states(sb_desktop_surface_t *desktop_surface)
{
    return desktop_surface->toplevel.states;
}

void sb_desktop_surface_set_wm_geometry(sb_desktop_surface_t *desktop_surface,
                                        const sb_rect_t *geometry)
{
    desktop_surface->wm_geometry = *geometry;

    if (desktop_surface->_xdg_surface == NULL) {
        sb_log_warn("XDG surface of the desktop surface is NULL!\n");
    }

    xdg_surface_set_window_geometry(desktop_surface->_xdg_surface,
        geometry->pos.x, geometry->pos.y,
        geometry->size.width, geometry->size.height);
}

const sb_size_t*
sb_desktop_surface_toplevel_minimum_size(sb_desktop_surface_t *desktop_surface)
{
    return &desktop_surface->toplevel.minimum_size;
}

void sb_desktop_surface_toplevel_set_minimum_size(
    sb_desktop_surface_t *desktop_surface, const sb_size_t *size)
{
    desktop_surface->toplevel.minimum_size = *size;

    xdg_toplevel_set_min_size(desktop_surface->_xdg_toplevel,
        size->width, size->height);
}

void sb_desktop_surface_toplevel_close(sb_desktop_surface_t *desktop_surface)
{
    // TODO: Free memory.

    sb_application_t *app = sb_application_instance();
    sb_application_unregister_desktop_surface(app, desktop_surface);
}

void sb_desktop_surface_toplevel_move(sb_desktop_surface_t *desktop_surface)
{
    sb_application_t *app = sb_application_instance();

    xdg_toplevel_move(desktop_surface->_xdg_toplevel,
        sb_application_wl_seat(app),
        sb_application_pointer_button_serial(app));
}

void sb_desktop_surface_toplevel_resize(sb_desktop_surface_t *desktop_surface,
    sb_desktop_surface_toplevel_resize_edge edge)
{
    sb_application_t *app = sb_application_instance();

    enum xdg_toplevel_resize_edge resize_edge = 0;
    switch (edge) {
    case SB_DESKTOP_SURFACE_TOPLEVEL_RESIZE_EDGE_NONE:
        resize_edge = XDG_TOPLEVEL_RESIZE_EDGE_NONE;
        break;
    case SB_DESKTOP_SURFACE_TOPLEVEL_RESIZE_EDGE_TOP:
        resize_edge = XDG_TOPLEVEL_RESIZE_EDGE_TOP;
        break;
    case SB_DESKTOP_SURFACE_TOPLEVEL_RESIZE_EDGE_BOTTOM:
        resize_edge = XDG_TOPLEVEL_RESIZE_EDGE_BOTTOM;
        break;
    case SB_DESKTOP_SURFACE_TOPLEVEL_RESIZE_EDGE_LEFT:
        resize_edge = XDG_TOPLEVEL_RESIZE_EDGE_LEFT;
        break;
    case SB_DESKTOP_SURFACE_TOPLEVEL_RESIZE_EDGE_TOP_LEFT:
        resize_edge = XDG_TOPLEVEL_RESIZE_EDGE_TOP_LEFT;
        break;
    case SB_DESKTOP_SURFACE_TOPLEVEL_RESIZE_EDGE_BOTTOM_LEFT:
        resize_edge = XDG_TOPLEVEL_RESIZE_EDGE_BOTTOM_LEFT;
        break;
    case SB_DESKTOP_SURFACE_TOPLEVEL_RESIZE_EDGE_RIGHT:
        resize_edge = XDG_TOPLEVEL_RESIZE_EDGE_RIGHT;
        break;
    case SB_DESKTOP_SURFACE_TOPLEVEL_RESIZE_EDGE_TOP_RIGHT:
        resize_edge = XDG_TOPLEVEL_RESIZE_EDGE_TOP_RIGHT;
        break;
    case SB_DESKTOP_SURFACE_TOPLEVEL_RESIZE_EDGE_BOTTOM_RIGHT:
        resize_edge = XDG_TOPLEVEL_RESIZE_EDGE_BOTTOM_RIGHT;
        break;
    default:
        resize_edge = XDG_TOPLEVEL_RESIZE_EDGE_NONE;
        break;
    }

    xdg_toplevel_resize(desktop_surface->_xdg_toplevel,
        sb_application_wl_seat(app),
        sb_application_pointer_button_serial(app),
        resize_edge);
}

void sb_desktop_surface_toplevel_set_maximized(
    sb_desktop_surface_t *desktop_surface)
{
    if (desktop_surface->_xdg_toplevel == NULL) {
        sb_log_warn("XDG toplevel of the desktop surface is NULL!\n");
    }
    xdg_toplevel_set_maximized(desktop_surface->_xdg_toplevel);
}

void sb_desktop_surface_toplevel_unset_maximized(
    sb_desktop_surface_t *desktop_surface)
{
    if (desktop_surface->_xdg_toplevel == NULL) {
        sb_log_warn("XDG toplevel of the desktop surface is NULL!\n");
    }
    xdg_toplevel_unset_maximized(desktop_surface->_xdg_toplevel);
}

void sb_desktop_surface_toplevel_set_minimized(
    sb_desktop_surface_t *desktop_surface)
{
    if (desktop_surface->_xdg_toplevel == NULL) {
        sb_log_warn("XDG toplevel of the desktop surface is NULL!\n");
        return;
    }
    xdg_toplevel_set_minimized(desktop_surface->_xdg_toplevel);
}

void sb_desktop_surface_popup_grab_for_button(
    sb_desktop_surface_t *desktop_surface)
{
    if (desktop_surface->xdg_popup == NULL) {
        return;
    }
    xdg_popup_grab(desktop_surface->xdg_popup,
        sb_application_wl_seat(sb_application_instance()),
        sb_application_pointer_button_serial(sb_application_instance()));

    sb_surface_attach(desktop_surface->_surface);
}

void sb_desktop_surface_popup_grab_for_key(
    sb_desktop_surface_t *desktop_surface)
{
    if (desktop_surface->xdg_popup == NULL) {
        return;
    }
    // TODO.
}

void sb_desktop_surface_free(sb_desktop_surface_t *desktop_surface)
{
    sb_surface_free(desktop_surface->_surface);

    free(desktop_surface);
}

void sb_desktop_surface_add_event_listener(
    sb_desktop_surface_t *desktop_surface,
    enum sb_event_type event_type,
    void (*listener)(sb_event_t*))
{
    sb_event_listener_tuple_t *tuple = sb_event_listener_tuple_new(
        event_type, listener);
    sb_list_push(desktop_surface->event_listeners, (void*)tuple);
}

void sb_desktop_surface_on_resize(sb_desktop_surface_t *desktop_surface,
                                  sb_event_t *event)
{
    _event_listener_filter_for_each(desktop_surface->event_listeners,
        SB_EVENT_TYPE_RESIZE, event);
}

void sb_desktop_surface_on_state_change(sb_desktop_surface_t *desktop_surface,
                                        sb_event_t *event)
{
    _event_listener_filter_for_each(desktop_surface->event_listeners,
        SB_EVENT_TYPE_STATE_CHANGE,
        event);
}

void sb_desktop_surface_on_show(sb_desktop_surface_t *desktop_surface,
                                sb_event_t *event)
{
    _event_listener_filter_for_each(desktop_surface->event_listeners,
        SB_EVENT_TYPE_SHOW,
        event);
}

void sb_desktop_surface_on_hide(sb_desktop_surface_t *desktop_surface,
                                sb_event_t *event)
{
    _event_listener_filter_for_each(desktop_surface->event_listeners,
        SB_EVENT_TYPE_HIDE,
        event);
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
    sb_desktop_surface_t *desktop_surface = (sb_desktop_surface_t*)data;
    // sb_application_t *app = sb_application_instance();

    // XDG configure event does not contain restore states.
    // Manually store the states as boolean flags and compare these at the
    // end of iteration.
    bool maximized = false;
    bool fullscreen = false;
    sb_desktop_surface_toplevel_state_flags curr_states = 0;

    void *it;
    wl_array_for_each(it, states) {
        enum xdg_toplevel_state state = *(enum xdg_toplevel_state*)it;
        sb_log_debug("wl_array_for_each() - state: %d, %dx%d\n", state, width, height);
        switch (state) {
        case XDG_TOPLEVEL_STATE_MAXIMIZED:
        {
            maximized = true;
            curr_states |= SB_DESKTOP_SURFACE_TOPLEVEL_STATE_MAXIMIZED;

            break;
        }
        case XDG_TOPLEVEL_STATE_FULLSCREEN:
            curr_states |= SB_DESKTOP_SURFACE_TOPLEVEL_STATE_FULLSCREEN;

            break;
        case XDG_TOPLEVEL_STATE_ACTIVATED:
            curr_states |= SB_DESKTOP_SURFACE_TOPLEVEL_STATE_ACTIVATED;

            break;
        case XDG_TOPLEVEL_STATE_RESIZING:
        {
            sb_log_debug("Resize %dx%d\n", width, height);
            curr_states |= SB_DESKTOP_SURFACE_TOPLEVEL_STATE_RESIZING;
            if (desktop_surface->toplevel.initial_resizing == false) {
                sb_surface_t *surface = desktop_surface->_surface;

                sb_size_t new_size;
                new_size.width = width;
                new_size.height = height;

                // Just send a resize event (to the desktop surface).
                sb_event_t *event = sb_event_new(
                    SB_EVENT_TARGET_TYPE_DESKTOP_SURFACE,
                    desktop_surface,
                    SB_EVENT_TYPE_RESIZE);
                event->resize.old_size = *sb_surface_size(surface);
                event->resize.size.width = width;
                event->resize.size.height = height;

                sb_application_post_event(sb_application_instance(), event);
            } else {
                // Ignore and set the initial is false.
                desktop_surface->toplevel.initial_resizing = false;
            }

            break;
        }
        default:
            break;
        }
    }

    // Compare states.
    {
        sb_desktop_surface_toplevel_state_flags states;
        states = desktop_surface->toplevel.states;

        // Maximized.
        if (!(states & SB_DESKTOP_SURFACE_TOPLEVEL_STATE_MAXIMIZED) &&
            curr_states & SB_DESKTOP_SURFACE_TOPLEVEL_STATE_MAXIMIZED) {
            desktop_surface->toplevel.states
                |= SB_DESKTOP_SURFACE_TOPLEVEL_STATE_MAXIMIZED;

            sb_event_t *event = _state_change_event_new(desktop_surface,
                SB_DESKTOP_SURFACE_TOPLEVEL_STATE_MAXIMIZED, true,
                width, height);

            sb_application_post_event(sb_application_instance(), event);
        } else if (states & SB_DESKTOP_SURFACE_TOPLEVEL_STATE_MAXIMIZED &&
            !(curr_states & SB_DESKTOP_SURFACE_TOPLEVEL_STATE_MAXIMIZED)) {
            desktop_surface->toplevel.states
                &= ~SB_DESKTOP_SURFACE_TOPLEVEL_STATE_MAXIMIZED;

            sb_event_t *event = _state_change_event_new(desktop_surface,
                SB_DESKTOP_SURFACE_TOPLEVEL_STATE_MAXIMIZED, false,
                width, height);

            sb_application_post_event(sb_application_instance(), event);
        }

        // Activated.
        if (!(states & SB_DESKTOP_SURFACE_TOPLEVEL_STATE_ACTIVATED) &&
            curr_states & SB_DESKTOP_SURFACE_TOPLEVEL_STATE_ACTIVATED) {
            desktop_surface->toplevel.states
                |= SB_DESKTOP_SURFACE_TOPLEVEL_STATE_ACTIVATED;

            sb_event_t *event = _state_change_event_new(desktop_surface,
                SB_DESKTOP_SURFACE_TOPLEVEL_STATE_ACTIVATED, true, 0, 0);

            sb_application_post_event(sb_application_instance(), event);
        } else if (states & SB_DESKTOP_SURFACE_TOPLEVEL_STATE_ACTIVATED &&
            !(curr_states & SB_DESKTOP_SURFACE_TOPLEVEL_STATE_ACTIVATED)) {
            desktop_surface->toplevel.states
                &= ~SB_DESKTOP_SURFACE_TOPLEVEL_STATE_ACTIVATED;

            sb_event_t *event = _state_change_event_new(desktop_surface,
                SB_DESKTOP_SURFACE_TOPLEVEL_STATE_ACTIVATED, false, 0, 0);

            sb_application_post_event(sb_application_instance(), event);
        }
    }
}

static void xdg_toplevel_close_handler(void *data,
                                       struct xdg_toplevel *xdg_toplevel)
{
    sb_desktop_surface_t *desktop_surface = (sb_desktop_surface_t*)data;

    sb_desktop_surface_toplevel_close(desktop_surface);
}

//!<================
//!< XDG Popup
//!<================

static void xdg_popup_configure_handler(void *data,
                                        struct xdg_popup *xdg_popup,
                                        int32_t x,
                                        int32_t y,
                                        int32_t width,
                                        int32_t height)
{
    sb_log_debug("xdg_popup_configure_handler\n");
}

static void xdg_popup_popup_done_handler(void *data,
                                         struct xdg_popup *xdg_popup)
{
    sb_desktop_surface_t *desktop_surface = (sb_desktop_surface_t*)data;
    sb_log_debug("xdg_popup_popup_done_handler()\n");
    sb_desktop_surface_hide(desktop_surface);
}

static void xdg_popup_repositioned_handler(void *data,
                                           struct xdg_popup *xdg_popup,
                                           uint32_t token)
{
    sb_log_debug("xdg_popup_repositioned_handler() - token: %d\n", token);
}
