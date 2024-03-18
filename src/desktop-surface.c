#include <foundation/desktop-surface.h>

#include <stdlib.h>

#include <wayland-protocols/stable/xdg-shell.h>

#include <foundation/surface.h>
#include <foundation/application.h>

struct ft_desktop_surface_t {
    ft_surface_t *_surface;
    ft_desktop_surface_role _role;
    struct xdg_surface *_xdg_surface;
    struct xdg_toplevel *_xdg_toplevel;
    struct xdg_popup *_xdg_popup;
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


ft_desktop_surface_t* ft_desktop_surface_new(ft_desktop_surface_role role)
{
    ft_desktop_surface_t *d_surface = malloc(sizeof(ft_desktop_surface_t));

    // Initialize the members.
    d_surface->_role = role;
    d_surface->_xdg_toplevel = NULL;
    d_surface->_xdg_popup = NULL;

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
            &xdg_toplevel_listener, NULL);
    } else if (desktop_surface->_role == FT_DESKTOP_SURFACE_ROLE_POPUP) {
        //
    }

    // Commit.
    wl_surface_commit(wl_surface);

    ft_surface_attach(desktop_surface->_surface);

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


static void xdg_surface_configure_handler(void *data,
                                          struct xdg_surface *xdg_surface,
                                          uint32_t serial)
{
    xdg_surface_ack_configure(xdg_surface, serial);
}


static void xdg_toplevel_configure_handler(void *data,
                                           struct xdg_toplevel *xdg_toplevel,
                                           int32_t width,
                                           int32_t height,
                                           struct wl_array *states)
{
    //
}

static void xdg_toplevel_close_handler(void *data,
                                       struct xdg_toplevel *xdg_toplevel)
{
    //
}
