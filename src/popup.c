#include <swingby/popup.h>

#include <stdlib.h>
#include <string.h>

#include <wayland-client.h>
#include <wayland-protocols/stable/xdg-shell.h>

#include <swingby/application.h>
#include <swingby/surface.h>
#include <swingby/surface-role.h>
#include <swingby/toplevel.h>
#include <swingby/rect.h>
#include <swingby/list.h>
#include <swingby/log.h>

#include "helpers/shared.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

struct sb_popup_t {
    sb_surface_role_header_t role;
    void *parent;
    struct xdg_surface *xdg_surface;
    struct xdg_popup *xdg_popup;
    sb_rect_i_t wm_geometry;
    sb_point_t position;
    bool grabbable;
    sb_list_t *event_listeners;
};

static void popup_on_event(sb_event_t *event, void *user_data)
{
    sb_popup_t *popup = user_data;

    _event_listener_filter_for_each(popup->event_listeners,
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


//!<==========
//!< Popup
//!<==========

sb_popup_t* sb_popup_new()
{
    sb_popup_t *popup = malloc(sizeof(sb_popup_t));

    popup->parent = NULL;

    popup->role.name = "popup";
    popup->role.surface = sb_surface_new();
    popup->role.event_filter = popup_on_event;

    popup->xdg_popup = NULL;

    popup->wm_geometry.position.x = 0;
    popup->wm_geometry.position.y = 0;
    popup->wm_geometry.size.width = 0;
    popup->wm_geometry.size.height = 0;
    popup->position.x = 0;
    popup->position.y = 0;

    popup->grabbable = false;

    popup->event_listeners = sb_list_new();

    sb_application_register_surface_role(sb_application_instance(),
        &popup->role);

    return popup;
}

void sb_popup_set_parent(sb_popup_t *popup, sb_surface_role_header_t *parent)
{
    popup->parent = parent;
}

sb_surface_t* sb_popup_surface(sb_popup_t *popup)
{
    return popup->role.surface;
}

sb_point_t sb_popup_position(const sb_popup_t *popup)
{
    return popup->position;
}

void sb_popup_set_position(sb_popup_t *popup, sb_point_t position)
{
    popup->position = position;
}

void sb_popup_show(sb_popup_t *popup)
{
    if (popup->parent == NULL) {
        sb_log_warn("sb_popup_t has no parent!\n");
        return;
    }

    // Wayland objects.
    sb_application_t *app = sb_application_instance();
    struct xdg_wm_base *xdg_wm_base = sb_application_xdg_wm_base(app);
    struct wl_surface *wl_surface = sb_surface_wl_surface(
        popup->role.surface);

    // Create XDG surface.
    struct xdg_surface *xdg_surface = xdg_wm_base_get_xdg_surface(xdg_wm_base,
        wl_surface);
    popup->xdg_surface = xdg_surface;
    xdg_surface_add_listener(popup->xdg_surface,
        &xdg_surface_listener, NULL);

    // Create a popup.
    if (true) {
        struct xdg_positioner *positioner = xdg_wm_base_create_positioner(
            xdg_wm_base);
        sb_size_i_t surface_size = sb_surface_size(popup->role.surface);
        // sb_size_i_t parent_size = sb_surface_size(
        //     popup->parent->surface);
        xdg_positioner_set_size(positioner,
            surface_size.width, surface_size.height);
        xdg_positioner_set_anchor(positioner, XDG_POSITIONER_ANCHOR_TOP_LEFT);
        int32_t x;
        int32_t y;
        {
            x = popup->position.x;
            y = popup->position.y;
        }
        xdg_positioner_set_anchor_rect(positioner, x, y,
            1, 1);
        xdg_positioner_set_gravity(positioner,
            XDG_POSITIONER_GRAVITY_BOTTOM_RIGHT);

        sb_surface_role_header_t *parent = popup->parent;
        if (strcmp(parent->name, "toplevel") == 0) {
            sb_toplevel_t *toplevel = (sb_toplevel_t*)parent;
            popup->xdg_popup = xdg_surface_get_popup(xdg_surface,
                sb_toplevel_xdg_surface(toplevel), positioner);
        } else if (strcmp(parent->name, "popup") == 0) {
            sb_popup_t *popup = (sb_popup_t*)parent;
            popup->xdg_popup = xdg_surface_get_popup(xdg_surface,
                ((sb_popup_t*)popup->parent)->xdg_surface, positioner);
        } else {
            sb_log_warn("sb_popup_show - parent must be toplevel or popup.\n");
        }

        xdg_popup_add_listener(popup->xdg_popup, &xdg_popup_listener,
            (void*)popup);

        xdg_positioner_destroy(positioner);
    }

    // Commit.

    // Show directly for a non grabbable popup.
    if (!popup->grabbable) {
        // Must commit and roundtrip.
        sb_surface_commit(popup->role.surface);
        wl_display_roundtrip(sb_application_wl_display(app));

        sb_surface_attach(popup->role.surface);
    }

    sb_event_t *event = sb_event_new(SB_EVENT_TARGET_TYPE_SURFACE_ROLE,
        popup, SB_EVENT_TYPE_SHOW);
    sb_application_post_event(sb_application_instance(), event);

    // TEST
    // sb_surface_on_request_update(desktop_surface->_surface);
}

void sb_popup_hide(sb_popup_t *popup)
{
    if (true) {
        xdg_popup_destroy(popup->xdg_popup);
        popup->xdg_popup = NULL;

        // sb_application_unregister_desktop_surface(sb_application_instance(),
        //     desktop_surface);
    }

    xdg_surface_destroy(popup->xdg_surface);
    popup->xdg_surface = NULL;

    sb_surface_detach(popup->role.surface);

    sb_event_t *event = sb_event_new(SB_EVENT_TARGET_TYPE_SURFACE_ROLE,
        popup, SB_EVENT_TYPE_HIDE);
    sb_application_post_event(sb_application_instance(), event);
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
    sb_popup_t *popup = (sb_popup_t*)data;
    sb_log_debug("xdg_popup_popup_done_handler()\n");
    sb_popup_hide(popup);
}

static void xdg_popup_repositioned_handler(void *data,
                                           struct xdg_popup *xdg_popup,
                                           uint32_t token)
{
    sb_log_debug("xdg_popup_repositioned_handler() - token: %d\n", token);
}


#ifdef __cplusplus
}
#endif // __cplusplus
