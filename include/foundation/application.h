#ifndef _FOUNDATION_APPLICATION_H
#define _FOUNDATION_APPLICATION_H

#include <wayland-client.h>

/// \brief A global application object.
typedef struct ft_application_t ft_application_t;

typedef struct ft_surface_t ft_surface_t;

ft_application_t* ft_application_new(int argc, char *argv[]);

ft_application_t* ft_application_instance();

/// \brief Get the `struct wl_display` of the application.
struct wl_display* ft_application_wl_display(
    ft_application_t *application);

/// \brief Get the `struct wl_compositor` of the application.
struct wl_compositor* ft_application_wl_compositor(
    ft_application_t *application);

/// \brief Get the `struct xdg_wm_base` of the application.
struct xdg_wm_base* ft_application_xdg_wm_base(
    ft_application_t *application);

int ft_application_exec(ft_application_t *application);

#endif /* _FOUNDATION_APPLICATION_H */
