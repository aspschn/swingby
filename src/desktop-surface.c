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
#include <swingby/surface-role.h>
#include <swingby/toplevel.h>
#include <swingby/popup.h>

struct sb_desktop_surface_t {
    enum sb_desktop_surface_role role_type;
    sb_toplevel_t *toplevel;
    sb_popup_t *popup;
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
    /*
    if (parent->_xdg_toplevel == NULL) {
        sb_log_warn("Parent must show before child show.\n");
        return;
    }
    xdg_toplevel_set_parent(child->_xdg_toplevel, parent->_xdg_toplevel);
    */
}

static void _event_listener_filter_for_each(sb_list_t *listeners,
                                            enum sb_event_type type,
                                            sb_event_t *event)
{
    uint64_t length = sb_list_length(listeners);
    for (uint64_t i = 0; i < length; ++i) {
        sb_event_listener_tuple_t *tuple = sb_list_at(listeners, i);
        if (tuple->type == type) {
            tuple->listener(event, tuple->user_data);
        }
    }
}


sb_desktop_surface_t* sb_desktop_surface_new(sb_desktop_surface_role role)
{
    sb_desktop_surface_t *d_surface = malloc(sizeof(sb_desktop_surface_t));

    if (role == SB_DESKTOP_SURFACE_ROLE_TOPLEVEL) {
        d_surface->role_type = SB_DESKTOP_SURFACE_ROLE_TOPLEVEL;
        d_surface->toplevel = sb_toplevel_new();
    } else if (role == SB_DESKTOP_SURFACE_ROLE_POPUP) {
        d_surface->role_type = SB_DESKTOP_SURFACE_ROLE_POPUP;
        d_surface->popup = sb_popup_new();
    }

    return d_surface;
}

void sb_desktop_surface_set_parent(sb_desktop_surface_t *desktop_surface,
                                   sb_desktop_surface_t *parent)
{
    // desktop_surface->parent = parent;
}

bool
sb_desktop_surface_is_toplevel(const sb_desktop_surface_t *desktop_surface)
{
    return desktop_surface->role_type == SB_DESKTOP_SURFACE_ROLE_TOPLEVEL;
}

bool
sb_desktop_surface_is_popup(const sb_desktop_surface_t *desktop_surface)
{
    return desktop_surface->role_type == SB_DESKTOP_SURFACE_ROLE_POPUP;
}

sb_surface_t*
sb_desktop_surface_surface(sb_desktop_surface_t *desktop_surface)
{
    if (desktop_surface->role_type == SB_DESKTOP_SURFACE_ROLE_TOPLEVEL) {
        const sb_surface_role_header_t *role =
            sb_surface_role_header(desktop_surface->toplevel);
        return role->surface;
    } else if (desktop_surface->role_type == SB_DESKTOP_SURFACE_ROLE_POPUP) {
        const sb_surface_role_header_t *role =
            sb_surface_role_header(desktop_surface->popup);
        return role->surface;
    }
}

void sb_desktop_surface_show(sb_desktop_surface_t *desktop_surface)
{
    if (desktop_surface->role_type == SB_DESKTOP_SURFACE_ROLE_TOPLEVEL) {
        sb_toplevel_show(desktop_surface->toplevel);
    } else {
        sb_popup_show(desktop_surface->popup);
    }
}

void sb_desktop_surface_hide(sb_desktop_surface_t *desktop_surface)
{
    if (desktop_surface->role_type == SB_DESKTOP_SURFACE_ROLE_TOPLEVEL) {
        sb_toplevel_hide(desktop_surface->toplevel);
    } else if (desktop_surface->role_type == SB_DESKTOP_SURFACE_ROLE_POPUP) {
        sb_popup_hide(desktop_surface->popup);
    }
}

sb_desktop_surface_toplevel_state_flags
sb_desktop_surface_toplevel_states(sb_desktop_surface_t *desktop_surface)
{
    return sb_toplevel_states(desktop_surface->toplevel);
}

void sb_desktop_surface_set_wm_geometry(sb_desktop_surface_t *desktop_surface,
                                        sb_rect_i_t geometry)
{
    if (desktop_surface->role_type == SB_DESKTOP_SURFACE_ROLE_TOPLEVEL) {
        sb_toplevel_set_wm_geometry(desktop_surface->toplevel, geometry);
    } else if (desktop_surface->role_type == SB_DESKTOP_SURFACE_ROLE_POPUP) {
        // sb_popup_set_wm_geometry(desktop_surface->popup, geometry);
    }
}

const sb_size_t*
sb_desktop_surface_toplevel_minimum_size(sb_desktop_surface_t *desktop_surface)
{
    sb_size_i_t size = sb_toplevel_minimum_size(desktop_surface->toplevel);
}

void sb_desktop_surface_toplevel_set_minimum_size(
    sb_desktop_surface_t *desktop_surface, const sb_size_t *size)
{
    sb_size_i_t size_i;
    size_i.width = (int32_t)size->width;
    size_i.height = (int32_t)size->height;
    sb_toplevel_set_minimum_size(desktop_surface->toplevel, size_i);
}

void sb_desktop_surface_toplevel_close(sb_desktop_surface_t *desktop_surface)
{
    // TODO: Free memory.

    sb_application_t *app = sb_application_instance();
    sb_application_unregister_surface_role(app,
        (sb_surface_role_header_t*)desktop_surface->toplevel);
}

void sb_desktop_surface_toplevel_move(sb_desktop_surface_t *desktop_surface)
{
    sb_toplevel_move(desktop_surface->toplevel);
}

void sb_desktop_surface_toplevel_resize(sb_desktop_surface_t *desktop_surface,
    sb_desktop_surface_toplevel_resize_edge edge)
{
    sb_application_t *app = sb_application_instance();

    sb_toplevel_resize(desktop_surface->toplevel, edge);
}

void sb_desktop_surface_toplevel_set_maximized(
    sb_desktop_surface_t *desktop_surface)
{
    sb_toplevel_set_maximized(desktop_surface->toplevel);
}

void sb_desktop_surface_toplevel_unset_maximized(
    sb_desktop_surface_t *desktop_surface)
{
    sb_toplevel_unset_maximized(desktop_surface->toplevel);
}

void sb_desktop_surface_toplevel_set_minimized(
    sb_desktop_surface_t *desktop_surface)
{
    sb_toplevel_set_minimized(desktop_surface->toplevel);
}

void sb_desktop_surface_toplevel_set_title(
    sb_desktop_surface_t *desktop_surface, const char *title)
{
    sb_toplevel_set_title(desktop_surface->toplevel, title);
}

void sb_desktop_surface_toplevel_set_app_id(
    sb_desktop_surface_t *desktop_surface, const char *app_id)
{
    sb_toplevel_set_app_id(desktop_surface->toplevel, app_id);
}

void sb_desktop_surface_toplevel_show_window_menu(
    sb_desktop_surface_t *desktop_surface, const sb_point_i_t *position)
{
    sb_toplevel_show_window_menu(desktop_surface->toplevel, *position);
}


void sb_desktop_surface_popup_set_position(
    sb_desktop_surface_t *desktop_surface, const sb_point_t *position)
{
    // desktop_surface->popup.position = *position;
}

void sb_desktop_surface_popup_set_grabbable(
    sb_desktop_surface_t *desktop_surface, bool value)
{
    // desktop_surface->popup.grabbable = value;
}

void sb_desktop_surface_popup_grab_for_button(
    sb_desktop_surface_t *desktop_surface)
{
    /*
    if (desktop_surface->xdg_popup == NULL) {
        return;
    }
    if (desktop_surface->popup.grabbable == false) {
        sb_log_warn("sb_desktop_surface_popup_grab_for_button"
                    " - Popup is not set as grabbable.\n");
        return;
    }

    xdg_popup_grab(desktop_surface->xdg_popup,
        sb_application_wl_seat(sb_application_instance()),
        sb_application_pointer_button_serial(sb_application_instance()));

    sb_surface_commit(desktop_surface->_surface);
    wl_display_roundtrip(sb_application_wl_display(sb_application_instance()));
    sb_surface_attach(desktop_surface->_surface);
    */
}

void sb_desktop_surface_popup_grab_for_key(
    sb_desktop_surface_t *desktop_surface)
{
    // TODO.
}

void sb_desktop_surface_free(sb_desktop_surface_t *desktop_surface)
{
    // sb_surface_free(desktop_surface->_surface);

    free(desktop_surface);
}

void sb_desktop_surface_add_event_listener(
    sb_desktop_surface_t *desktop_surface,
    enum sb_event_type event_type,
    sb_event_listener_t listener,
    void *user_data)
{
    if (desktop_surface->role_type == SB_DESKTOP_SURFACE_ROLE_TOPLEVEL) {
        sb_toplevel_add_event_listener(
            desktop_surface->toplevel,
            event_type,
            listener,
            user_data);
    } else if (desktop_surface->role_type == SB_DESKTOP_SURFACE_ROLE_POPUP) {
        // sb_popup_add_event_listener();
    }
}
