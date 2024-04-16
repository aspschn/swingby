#ifndef _FOUNDATION_SURFACE_H
#define _FOUNDATION_SURFACE_H

#include <wayland-client.h>

#include <swingby/size.h>
#include <swingby/event.h>

typedef struct ft_rect_t ft_rect_t;
typedef struct ft_view_t ft_view_t;
typedef struct ft_event_t ft_event_t;

/// \brief A basic rectangular region.
typedef struct ft_surface_t ft_surface_t;

ft_surface_t* ft_surface_new();

void ft_surface_set_wl_surface(ft_surface_t *surface,
                               struct wl_surface *wl_surface);

const ft_size_t* ft_surface_size(ft_surface_t *surface);

/// \brief Set the surface size.
///
/// Surface size is always integer values. Values below the decimal point
/// may cause undefined behavior.
void ft_surface_set_size(ft_surface_t *surface, const ft_size_t *size);

ft_view_t* ft_surface_root_view(ft_surface_t *surface);

void ft_surface_commit(ft_surface_t *surface);

void ft_surface_attach(ft_surface_t *surface);

void ft_surface_detach(ft_surface_t *surface);

void ft_surface_update(ft_surface_t *surface);

/// \brief Set the valid input geometry of the surface.
///
/// Pointer events outside of the input geoemetry will be ignored.
/// This is useful to draw the window's shadow. The shadow still a part of
/// the surface in CSD but click the shadow should not activate the window.
void ft_surface_set_input_geometry(ft_surface_t *surface, ft_rect_t *geometry);

void ft_surface_add_event_listener(ft_surface_t *surface,
                                   enum ft_event_type event_type,
                                   void (*listener)(ft_event_t*));

void ft_surface_on_pointer_enter(ft_surface_t *surface, ft_event_t *event);

void ft_surface_on_pointer_leave(ft_surface_t *surface, ft_event_t *event);

void ft_surface_on_request_update(ft_surface_t *surface);

void ft_surface_on_resize(ft_surface_t *surface, ft_event_t *event);

struct wl_surface* ft_surface_wl_surface(ft_surface_t *surface);

#endif /* _FOUNDATION_SURFACE_H */
