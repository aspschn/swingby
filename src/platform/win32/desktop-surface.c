#include <swingby/desktop-surface.h>

#include <stdbool.h>
#include <stdlib.h>

#include <Windows.h>

#include <swingby/rect.h>
#include <swingby/list.h>
#include <swingby/surface.h>
#include <swingby/application.h>
#include <swingby/log.h>

#include "../../helpers/shared.h"

struct sb_desktop_surface_t {
    sb_surface_t *surface;
    enum sb_desktop_surface_role role;
    sb_desktop_surface_t *parent;
    sb_rect_t wm_geometry;
    struct {
        bool moving;
        bool resizing;
    } toplevel;
    sb_list_t *event_listeners;
};

//!<=====================
//!< Helper Functions
//!<=====================


//!<=====================
//!< Desktop Surface
//!<=====================

sb_desktop_surface_t* sb_desktop_surface_new(enum sb_desktop_surface_role role)
{
    sb_desktop_surface_t *desktop_surface =
        malloc(sizeof(sb_desktop_surface_t));

    desktop_surface->role = role;

    // NULL initializations.
    desktop_surface->surface = NULL;
    desktop_surface->parent = NULL;

    desktop_surface->wm_geometry.pos.x = 0;
    desktop_surface->wm_geometry.pos.y = 0;
    desktop_surface->wm_geometry.size.width = 0;
    desktop_surface->wm_geometry.size.height = 0;

    desktop_surface->toplevel.moving = false;
    desktop_surface->toplevel.resizing = false;

    // Create a surface.
    desktop_surface->surface = sb_surface_new();
    sb_log_debug("sb_desktop_surface_new - Surface created.\n");

    desktop_surface->event_listeners = sb_list_new();

    sb_application_register_desktop_surface(
        sb_application_instance(),
        desktop_surface
    );

    return desktop_surface;
}

sb_surface_t* sb_desktop_surface_surface(sb_desktop_surface_t *desktop_surface)
{
    return desktop_surface->surface;
}

void sb_desktop_surface_show(sb_desktop_surface_t *desktop_surface)
{
    sb_surface_attach(desktop_surface->surface);
    sb_surface_update(desktop_surface->surface);

    ShowWindow(sb_surface_hwnd(desktop_surface->surface), SW_SHOWDEFAULT);
    sb_log_debug("Desktop surface show\n");
}

void sb_desktop_surface_hide(sb_desktop_surface_t *desktop_surface)
{
    // TODO.
}

sb_desktop_surface_toplevel_state_flags
sb_desktop_surface_toplevel_states(sb_desktop_surface_t *desktop_surface)
{
    return SB_DESKTOP_SURFACE_TOPLEVEL_STATE_NORMAL;
}

void sb_desktop_surface_set_wm_geometry(sb_desktop_surface_t *desktop_surface,
                                        const sb_rect_t *geometry)
{
    // TODO.
}

void sb_desktop_surface_toplevel_close(sb_desktop_surface_t *desktop_surface)
{
    sb_application_set_nchittest_return(sb_application_instance(),
        HTCLOSE);
}

void sb_desktop_surface_toplevel_move(sb_desktop_surface_t *desktop_surface)
{
    desktop_surface->toplevel.moving = true;    // Maybe not used.
    sb_log_debug("sb_desktop_surface_toplevel_move()\n");
    sb_application_set_nchittest_return(sb_application_instance(),
        HTCAPTION);
}

void sb_desktop_surface_toplevel_resize(sb_desktop_surface_t *desktop_surface,
    sb_desktop_surface_toplevel_resize_edge edge)
{
    int param = HTBOTTOMRIGHT;

    switch (edge) {
    case SB_DESKTOP_SURFACE_TOPLEVEL_RESIZE_EDGE_TOP:
        param = HTTOP;
        break;
    case SB_DESKTOP_SURFACE_TOPLEVEL_RESIZE_EDGE_BOTTOM:
        param = HTBOTTOM;
        break;
    case SB_DESKTOP_SURFACE_TOPLEVEL_RESIZE_EDGE_LEFT:
        param = HTLEFT;
        break;
    case SB_DESKTOP_SURFACE_TOPLEVEL_RESIZE_EDGE_TOP_LEFT:
        param = HTTOPLEFT;
    case SB_DESKTOP_SURFACE_TOPLEVEL_RESIZE_EDGE_BOTTOM_LEFT:
        param = HTBOTTOMLEFT;
        break;
    case SB_DESKTOP_SURFACE_TOPLEVEL_RESIZE_EDGE_RIGHT:
        param = HTRIGHT;
        break;
    case SB_DESKTOP_SURFACE_TOPLEVEL_RESIZE_EDGE_TOP_RIGHT:
        param = HTTOPRIGHT;
        break;
    case SB_DESKTOP_SURFACE_TOPLEVEL_RESIZE_EDGE_BOTTOM_RIGHT:
        param = HTBOTTOMRIGHT;
        break;
    default:
        break;
    }
    sb_application_set_nchittest_return(sb_application_instance(),
        param);
}

void sb_desktop_surface_toplevel_set_maximized(
    sb_desktop_surface_t *desktop_surface)
{
    // TODO.
}

void sb_desktop_surface_toplevel_unset_maximized(
    sb_desktop_surface_t *desktop_surface)
{
    // TODO.
}

void sb_desktop_surface_toplevel_set_minimized(
    sb_desktop_surface_t *desktop_surface)
{
    // TODO.
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
}
