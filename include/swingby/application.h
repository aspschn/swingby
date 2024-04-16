#ifndef _FOUNDATION_APPLICATION_H
#define _FOUNDATION_APPLICATION_H

#include <wayland-client.h>

typedef struct sb_surface_t sb_surface_t;
typedef struct sb_desktop_surface_t sb_desktop_surface_t;
typedef struct sb_event_t sb_event_t;

/// \brief A global application object.
typedef struct sb_application_t sb_application_t;

sb_application_t* sb_application_new(int argc, char *argv[]);

sb_application_t* sb_application_instance();

/// \brief Register a desktop surface.
///
/// A desktop surface must be registered to the application. This method is
/// for creation of a desktop surface.
void sb_application_register_desktop_surface(sb_application_t *application,
    sb_desktop_surface_t *desktop_surface);

/// \brief Unregister a desktop surface.
void sb_application_unregister_desktop_surface(sb_application_t *application,
    sb_desktop_surface_t *desktop_surface);

uint32_t sb_application_pointer_button_serial(sb_application_t *application);

void sb_application_post_event(sb_application_t *application,
                               sb_event_t *event);

/// \brief Get the `struct wl_display` of the application.
struct wl_display* sb_application_wl_display(
    sb_application_t *application);

/// \brief Get the `struct wl_compositor` of the application.
struct wl_compositor* sb_application_wl_compositor(
    sb_application_t *application);

/// \brief Get the `struct xdg_wm_base` of the application.
struct xdg_wm_base* sb_application_xdg_wm_base(
    sb_application_t *application);

/// \brief Get the `struct wl_seat` of the application.
struct wl_seat* sb_application_wl_seat(sb_application_t *application);

int sb_application_exec(sb_application_t *application);

#endif /* _FOUNDATION_APPLICATION_H */
