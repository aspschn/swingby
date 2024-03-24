#include <foundation/desktop-surface.h>

#include <stdbool.h>
#include <stdlib.h>

#include <wayland-client.h>
#include <wayland-protocols/stable/xdg-shell.h>

#include <foundation/log.h>
#include <foundation/size.h>
#include <foundation/rect.h>
#include <foundation/surface.h>
#include <foundation/application.h>
#include <foundation/list.h>
#include <foundation/event.h>

struct ft_desktop_surface_t {
    ft_surface_t *_surface;
    ft_desktop_surface_role _role;
    struct xdg_surface *_xdg_surface;
    struct xdg_toplevel *_xdg_toplevel;
    struct xdg_popup *_xdg_popup;
    ft_rect_i_t wm_geometry;
    struct {
        ft_size_i_t minimum_size;
        enum ft_desktop_surface_toplevel_state state;
        /// \brief Initial resizing configure event gives me the garbage values.
        ///
        /// Ignore first resizing event since it contains garbage values.
        /// The reason why is IDK. However ignore this and it will works anyway.
        bool initial_resizing;
    } toplevel;
    ft_list_t *event_listeners;
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

//!<=====================
//!< Helper Functions
//!<=====================

static void _event_listener_filter_for_each(ft_list_t *listeners,
                                            enum ft_event_type type,
                                            ft_event_t *event)
{
    uint64_t length = ft_list_length(listeners);
    for (uint64_t i = 0; i < length; ++i) {
        ft_event_listener_tuple_t *tuple = ft_list_at(listeners, i);
        if (tuple->type == type) {
            tuple->listener(event);
        }
    }
}


ft_desktop_surface_t* ft_desktop_surface_new(ft_desktop_surface_role role)
{
    ft_desktop_surface_t *d_surface = malloc(sizeof(ft_desktop_surface_t));

    // Initialize the members.
    d_surface->_role = role;
    d_surface->_xdg_toplevel = NULL;
    d_surface->_xdg_popup = NULL;

    d_surface->wm_geometry.pos.x = 0;
    d_surface->wm_geometry.pos.y = 0;
    d_surface->wm_geometry.size.width = 0;
    d_surface->wm_geometry.size.height = 0;

    d_surface->toplevel.state = FT_DESKTOP_SURFACE_TOPLEVEL_STATE_NORMAL;
    d_surface->toplevel.initial_resizing = true;

    d_surface->event_listeners = ft_list_new();

    // Create a surface.
    d_surface->_surface = ft_surface_new();

    ft_application_register_desktop_surface(ft_application_instance(),
        d_surface);

    return d_surface;
}

ft_surface_t*
ft_desktop_surface_surface(ft_desktop_surface_t *desktop_surface)
{
    return desktop_surface->_surface;
}

void ft_desktop_surface_show(ft_desktop_surface_t *desktop_surface)
{
    // Wayland objects.
    ft_application_t *app = ft_application_instance();
    struct xdg_wm_base *xdg_wm_base = ft_application_xdg_wm_base(app);
    struct wl_surface *wl_surface = ft_surface_wl_surface(
        desktop_surface->_surface);

    // Create XDG surface.
    struct xdg_surface *xdg_surface = xdg_wm_base_get_xdg_surface(xdg_wm_base,
        wl_surface);
    desktop_surface->_xdg_surface = xdg_surface;
    xdg_surface_add_listener(desktop_surface->_xdg_surface,
        &xdg_surface_listener, NULL);

    // Create toplevel or popup.
    if (desktop_surface->_role == FT_DESKTOP_SURFACE_ROLE_TOPLEVEL) {
        desktop_surface->_xdg_toplevel = xdg_surface_get_toplevel(xdg_surface);
        xdg_toplevel_add_listener(desktop_surface->_xdg_toplevel,
            &xdg_toplevel_listener, (void*)desktop_surface);

        // Must commit and roundtrip.
        ft_surface_commit(ft_desktop_surface_surface(desktop_surface));
        wl_display_roundtrip(ft_application_wl_display(app));

        // Set minimum size.
        ft_size_i_t min_size;
        min_size.width = 100;
        min_size.height = 100;
        ft_desktop_surface_toplevel_set_minimum_size(desktop_surface,
            &min_size);
    } else if (desktop_surface->_role == FT_DESKTOP_SURFACE_ROLE_POPUP) {
        //
    }

    // Commit.
    wl_surface_commit(wl_surface);

    ft_surface_attach(desktop_surface->_surface);

    ft_surface_update(desktop_surface->_surface);

    // TEST
    // ft_surface_on_request_update(desktop_surface->_surface);
}

void ft_desktop_surface_hide(ft_desktop_surface_t *desktop_surface)
{
    if (desktop_surface->_role == FT_DESKTOP_SURFACE_ROLE_TOPLEVEL) {
        xdg_toplevel_destroy(desktop_surface->_xdg_toplevel);
    } else if (desktop_surface->_role == FT_DESKTOP_SURFACE_ROLE_POPUP) {
        //
    }

    xdg_surface_destroy(desktop_surface->_xdg_surface);

    ft_surface_detach(desktop_surface->_surface);
}

void ft_desktop_surface_set_wm_geometry(ft_desktop_surface_t *desktop_surface,
                                        const ft_rect_i_t *geometry)
{
    desktop_surface->wm_geometry = *geometry;

    xdg_surface_set_window_geometry(desktop_surface->_xdg_surface,
        geometry->pos.x, geometry->pos.y,
        geometry->size.width, geometry->size.height);

    // Set input region.
    ft_application_t *app = ft_application_instance();
    ft_surface_t *surface = desktop_surface->_surface;
    struct wl_surface *wl_surface = ft_surface_wl_surface(surface);
    struct wl_compositor *wl_compositor = ft_application_wl_compositor(app);

    struct wl_region *region = wl_compositor_create_region(wl_compositor);
    wl_region_add(region,
                  geometry->pos.x, geometry->pos.y,
                  geometry->size.width, geometry->size.height);
    wl_surface_set_input_region(wl_surface, region);
    wl_region_destroy(region);
}

const ft_size_i_t*
ft_desktop_surface_toplevel_minimum_size(ft_desktop_surface_t *desktop_surface)
{
    return &desktop_surface->toplevel.minimum_size;
}

void ft_desktop_surface_toplevel_set_minimum_size(
    ft_desktop_surface_t *desktop_surface, const ft_size_i_t *size)
{
    desktop_surface->toplevel.minimum_size = *size;

    xdg_toplevel_set_min_size(desktop_surface->_xdg_toplevel,
        size->width, size->height);
}

void ft_desktop_surface_toplevel_close(ft_desktop_surface_t *desktop_surface)
{
    // TODO: Free memory.

    ft_application_t *app = ft_application_instance();
    ft_application_unregister_desktop_surface(app, desktop_surface);
}

void ft_desktop_surface_toplevel_move(ft_desktop_surface_t *desktop_surface)
{
    ft_application_t *app = ft_application_instance();

    xdg_toplevel_move(desktop_surface->_xdg_toplevel,
        ft_application_wl_seat(app),
        ft_application_pointer_button_serial(app));
}

void ft_desktop_surface_toplevel_resize(ft_desktop_surface_t *desktop_surface,
    ft_desktop_surface_toplevel_resize_edge edge)
{
    ft_application_t *app = ft_application_instance();

    enum xdg_toplevel_resize_edge resize_edge = 0;
    switch (edge) {
    case FT_DESKTOP_SURFACE_TOPLEVEL_RESIZE_EDGE_NONE:
        resize_edge = XDG_TOPLEVEL_RESIZE_EDGE_NONE;
        break;
    case FT_DESKTOP_SURFACE_TOPLEVEL_RESIZE_EDGE_TOP:
        resize_edge = XDG_TOPLEVEL_RESIZE_EDGE_TOP;
        break;
    case FT_DESKTOP_SURFACE_TOPLEVEL_RESIZE_EDGE_BOTTOM:
        resize_edge = XDG_TOPLEVEL_RESIZE_EDGE_BOTTOM;
        break;
    case FT_DESKTOP_SURFACE_TOPLEVEL_RESIZE_EDGE_LEFT:
        resize_edge = XDG_TOPLEVEL_RESIZE_EDGE_LEFT;
        break;
    case FT_DESKTOP_SURFACE_TOPLEVEL_RESIZE_EDGE_TOP_LEFT:
        resize_edge = XDG_TOPLEVEL_RESIZE_EDGE_TOP_LEFT;
        break;
    case FT_DESKTOP_SURFACE_TOPLEVEL_RESIZE_EDGE_BOTTOM_LEFT:
        resize_edge = XDG_TOPLEVEL_RESIZE_EDGE_BOTTOM_LEFT;
        break;
    case FT_DESKTOP_SURFACE_TOPLEVEL_RESIZE_EDGE_RIGHT:
        resize_edge = XDG_TOPLEVEL_RESIZE_EDGE_RIGHT;
        break;
    case FT_DESKTOP_SURFACE_TOPLEVEL_RESIZE_EDGE_TOP_RIGHT:
        resize_edge = XDG_TOPLEVEL_RESIZE_EDGE_TOP_RIGHT;
        break;
    case FT_DESKTOP_SURFACE_TOPLEVEL_RESIZE_EDGE_BOTTOM_RIGHT:
        resize_edge = XDG_TOPLEVEL_RESIZE_EDGE_BOTTOM_RIGHT;
        break;
    default:
        resize_edge = XDG_TOPLEVEL_RESIZE_EDGE_NONE;
        break;
    }

    xdg_toplevel_resize(desktop_surface->_xdg_toplevel,
        ft_application_wl_seat(app),
        ft_application_pointer_button_serial(app),
        resize_edge);
}

void ft_desktop_surface_add_event_listener(
    ft_desktop_surface_t *desktop_surface,
    enum ft_event_type event_type,
    void (*listener)(ft_event_t*))
{
    ft_event_listener_tuple_t *tuple = ft_event_listener_tuple_new(
        event_type, listener);
    ft_list_push(desktop_surface->event_listeners, (void*)tuple);
}

void ft_desktop_surface_on_resize(ft_desktop_surface_t *desktop_surface,
                                  ft_event_t *event)
{
    _event_listener_filter_for_each(desktop_surface->event_listeners,
        FT_EVENT_TYPE_RESIZE, event);
}

void ft_desktop_surface_on_state_change(ft_desktop_surface_t *desktop_surface,
                                        ft_event_t *event)
{
    _event_listener_filter_for_each(desktop_surface->event_listeners,
        FT_EVENT_TYPE_STATE_CHANGE,
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
    ft_desktop_surface_t *desktop_surface = (ft_desktop_surface_t*)data;
    // ft_application_t *app = ft_application_instance();

    void *it;
    wl_array_for_each(it, states) {
        enum xdg_toplevel_state state = *(enum xdg_toplevel_state*)it;
        switch (state) {
        case XDG_TOPLEVEL_STATE_MAXIMIZED:
        {
            int state = desktop_surface->toplevel.state;

            if (state != FT_DESKTOP_SURFACE_TOPLEVEL_STATE_MAXIMIZED) {
                desktop_surface->toplevel.state =
                    FT_DESKTOP_SURFACE_TOPLEVEL_STATE_MAXIMIZED;

                ft_event_t *event = ft_event_new(
                    FT_EVENT_TARGET_TYPE_DESKTOP_SURFACE,
                    desktop_surface,
                    FT_EVENT_TYPE_STATE_CHANGE);
                event->state_change.state =
                    FT_DESKTOP_SURFACE_TOPLEVEL_STATE_MAXIMIZED;
                event->state_change.size.width = width;
                event->state_change.size.height = height;

                ft_application_post_event(ft_application_instance(), event);
            }

            break;
        }
        case XDG_TOPLEVEL_STATE_RESIZING:
        {
            ft_log_debug("Resize %dx%d\n", width, height);
            if (desktop_surface->toplevel.initial_resizing == false) {
                ft_surface_t *surface = desktop_surface->_surface;

                ft_size_t new_size;
                new_size.width = width;
                new_size.height = height;

                // Just send a resize event (to the desktop surface).
                ft_event_t *event = ft_event_new(
                    FT_EVENT_TARGET_TYPE_DESKTOP_SURFACE,
                    desktop_surface,
                    FT_EVENT_TYPE_RESIZE);
                event->resize.old_size = *ft_surface_size(surface);
                event->resize.size.width = width;
                event->resize.size.height = height;

                ft_application_post_event(ft_application_instance(), event);
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
}

static void xdg_toplevel_close_handler(void *data,
                                       struct xdg_toplevel *xdg_toplevel)
{
    ft_desktop_surface_t *desktop_surface = (ft_desktop_surface_t*)data;

    ft_desktop_surface_toplevel_close(desktop_surface);
}
