#ifndef _FOUNDATION_SURFACE_H
#define _FOUNDATION_SURFACE_H

#include <wayland-client.h>

#include <swingby/size.h>
#include <swingby/event.h>

typedef struct sb_rect_t sb_rect_t;
typedef struct sb_view_t sb_view_t;
typedef struct sb_event_t sb_event_t;

/// \brief A basic rectangular region.
typedef struct sb_surface_t sb_surface_t;

sb_surface_t* sb_surface_new();

void sb_surface_set_wl_surface(sb_surface_t *surface,
                               struct wl_surface *wl_surface);

const sb_size_t* sb_surface_size(sb_surface_t *surface);

/// \brief Set the surface size.
///
/// Surface size is always integer values. Values below the decimal point
/// may cause undefined behavior.
void sb_surface_set_size(sb_surface_t *surface, const sb_size_t *size);

sb_view_t* sb_surface_root_view(sb_surface_t *surface);

void sb_surface_commit(sb_surface_t *surface);

void sb_surface_attach(sb_surface_t *surface);

void sb_surface_detach(sb_surface_t *surface);

void sb_surface_update(sb_surface_t *surface);

/// \brief Set the valid input geometry of the surface.
///
/// Pointer events outside of the input geoemetry will be ignored.
/// This is useful to draw the window's shadow. The shadow still a part of
/// the surface in CSD but click the shadow should not activate the window.
void sb_surface_set_input_geometry(sb_surface_t *surface, sb_rect_t *geometry);

void sb_surface_add_event_listener(sb_surface_t *surface,
                                   enum sb_event_type event_type,
                                   void (*listener)(sb_event_t*));

void sb_surface_on_pointer_enter(sb_surface_t *surface, sb_event_t *event);

void sb_surface_on_pointer_leave(sb_surface_t *surface, sb_event_t *event);

void sb_surface_on_request_update(sb_surface_t *surface);

void sb_surface_on_resize(sb_surface_t *surface, sb_event_t *event);

struct wl_surface* sb_surface_wl_surface(sb_surface_t *surface);

#endif /* _FOUNDATION_SURFACE_H */
