#include <foundation/application.h>

#include <stdlib.h>
#include <string.h>

#include <wayland-client.h>
#include <wayland-protocols/stable/xdg-shell.h>

#include <foundation/surface.h>

struct ft_application_t {
    struct wl_display *_wl_display;
    struct wl_registry *_wl_registry;
    struct wl_compositor *_wl_compositor;
    struct xdg_wm_base *_xdg_wm_base;
    struct wl_seat *_wl_seat;
    struct wl_pointer *_wl_pointer;
    struct wl_keyboard *_wl_keyboard;
    struct wl_touch *_wl_touch;
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


ft_application_t* ft_application_new(int argc, char *argv[])
{
    ft_application_t *app = malloc(sizeof(ft_application_t));

    app->_wl_display = wl_display_connect(NULL);

    // Null initializations.
    app->_wl_seat = NULL;
    app->_wl_pointer = NULL;
    app->_wl_keyboard = NULL;
    app->_wl_touch = NULL;

    app->_wl_registry = wl_display_get_registry(app->_wl_display);
    wl_registry_add_listener(app->_wl_registry, &app_registry_listener,
        (void*)app);

    wl_display_dispatch(app->_wl_display);
    wl_display_roundtrip(app->_wl_display);

    xdg_wm_base_add_listener(app->_xdg_wm_base, &app_xdg_wm_base_listener,
        NULL);

    _ft_application_instance = app;

    return app;
}

ft_application_t* ft_application_instance()
{
    return _ft_application_instance;
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

int ft_application_exec(ft_application_t *application)
{
    while (wl_display_dispatch(application->_wl_display) != -1) {
        ;
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
                                  struct wl_surface *surface,
                                  wl_fixed_t sx,
                                  wl_fixed_t sy)
{
    //
}

static void pointer_motion_handler(void *data,
                                   struct wl_pointer *wl_pointer,
                                   uint32_t time,
                                   wl_fixed_t sx,
                                   wl_fixed_t sy)
{
    //
}

static void pointer_button_handler(void *data,
                                   struct wl_pointer *pointer,
                                   uint32_t serial,
                                   uint32_t time,
                                   uint32_t button,
                                   uint32_t state)
{
    //
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
    //
}

static void seat_name_handler(void *data,
                              struct wl_seat *wl_seat,
                              const char *name)
{
    //
}
