#ifndef _SWINBGY_APPLICATION_H
#define _SWINBGY_APPLICATION_H

#include <stdbool.h>
#include <stdint.h>

#if defined(SB_PLATFORM_WIN32)
#include <Windows.h>
#else
#define WNDCLASS void
#endif

#include <swingby/common.h>
#include <swingby/event.h>

typedef struct sb_surface_t sb_surface_t;
typedef struct sb_desktop_surface_t sb_desktop_surface_t;
typedef struct sb_event_t sb_event_t;
typedef struct sb_list_t sb_list_t;

typedef struct sb_egl_t sb_egl_t;
typedef sb_egl_t sb_egl_context_t;
typedef struct sb_d3d_global_context_t sb_d3d_global_context_t;

/// \struct sb_application_t
/// \brief A global application object.
typedef struct sb_application_t sb_application_t;

/// \memberof sb_application_t
/// \brief Create new singleton application object.
///
/// This method must call only once per application.
///
/// \param argc `argc` from `int main`.
/// \param argv `argv` from `int main`.
/// \return Created application object.
SB_EXPORT
sb_application_t* sb_application_new(int argc, char *argv[]);

/// \memberof sb_application_t
/// \brief Get the singleton instance.
///
/// \return Pointer to global application.
SB_EXPORT
sb_application_t* sb_application_instance();

/// \memberof sb_application_t
/// \brief Register a desktop surface.
///
/// A desktop surface must be registered to the application. This method is
/// for creation of a desktop surface.
void sb_application_register_desktop_surface(sb_application_t *application,
    sb_desktop_surface_t *desktop_surface);

/// \memberof sb_application_t
/// \brief Unregister a desktop surface.
void sb_application_unregister_desktop_surface(sb_application_t *application,
    sb_desktop_surface_t *desktop_surface);

/// \memberof sb_application_t
uint32_t sb_application_pointer_button_serial(sb_application_t *application);

/// \memberof sb_application_t
/// \brief Load cursor themes. If already loaded, reload themes.
void sb_application_load_cursor_themes(sb_application_t *application);

/// \memberof sb_application_t
const sb_list_t* sb_application_cursor_theme_ids(sb_application_t *application);

/// \memberof sb_application_t
/// \brief Set current cursor theme by id.
void sb_application_set_cursor_theme(sb_application_t *application,
                                     const char *id);

/// \memberof sb_application_t
/// \brief Return current cursor theme id. If not set before then return NULL.
const char* sb_application_cursor_theme(sb_application_t *application);

uint32_t sb_application_add_timer(sb_application_t *application,
                                  sb_surface_t *target,
                                  uint32_t interval,
                                  bool repeat);

/// \memberof sb_application_t
/// \brief Remove timer event by id.
void sb_application_remove_timer(sb_application_t *application, uint32_t id);

/// \memberof sb_application_t
void sb_application_post_event(sb_application_t *application,
                               sb_event_t *event);

/// \memberof sb_application_t
/// \brief Get the `struct wl_display` of the application.
struct wl_display* sb_application_wl_display(
    sb_application_t *application);

/// \memberof sb_application_t
/// \brief Get the `struct wl_compositor` of the application.
struct wl_compositor* sb_application_wl_compositor(
    sb_application_t *application);

/// \memberof sb_application_t
/// \brief Get the `struct wl_shm` of the application.
struct wl_shm* sb_application_wl_shm(sb_application_t *application);

/// \memberof sb_application_t
/// \brief Get the `struct xdg_wm_base` of the application.
struct xdg_wm_base* sb_application_xdg_wm_base(
    sb_application_t *application);

/// \memberof sb_application_t
/// \brief Get the `struct wl_seat` of the application.
struct wl_seat* sb_application_wl_seat(sb_application_t *application);

/// \memberof sb_application_t
struct zwp_text_input_v3* sb_application_zwp_text_input_v3(
    sb_application_t *application);

/// \memberof sb_application_t
sb_egl_context_t* sb_application_egl_context(sb_application_t *application)
    __attribute__((deprecated("use sb_application_egl() instead.")));

/// \memberof sb_application_t
sb_egl_t* sb_application_egl(sb_application_t *application);

/// \memberof sb_application_t
/// \brief Run the main event loop.
///
/// \return 0 when an application quit without error.
///         Commonly used to pass return value of a main function.
SB_EXPORT
int sb_application_exec(sb_application_t *application);

/// \memberof sb_application_t
SB_EXPORT
void sb_application_add_event_listener(sb_application_t *application,
                                       enum sb_event_type event_type,
                                       sb_event_listener_t listener,
                                       void *user_data);

/// \memberof sb_application_t
SB_EXPORT
void sb_application_on_next_tick(sb_application_t *application,
                                 sb_event_t *event);

#endif /* _SWINBGY_APPLICATION_H */
