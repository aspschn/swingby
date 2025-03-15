#ifndef _FOUNDATION_DESKTOP_SURFACE_H
#define _FOUNDATION_DESKTOP_SURFACE_H

#include <swingby/common.h>
#include <swingby/event.h>

typedef struct sb_size_i_t sb_size_i_t;
typedef struct sb_rect_i_t sb_rect_i_t;
typedef struct sb_size_t sb_size_t;
typedef struct sb_rect_t sb_rect_t;
typedef struct sb_surface_t sb_surface_t;

typedef enum sb_desktop_surface_role {
    SB_DESKTOP_SURFACE_ROLE_TOPLEVEL    = 0,
    SB_DESKTOP_SURFACE_ROLE_POPUP       = 1,
} sb_desktop_surface_role;

typedef enum sb_desktop_surface_toplevel_resize_edge {
    SB_DESKTOP_SURFACE_TOPLEVEL_RESIZE_EDGE_NONE            = 0,
    SB_DESKTOP_SURFACE_TOPLEVEL_RESIZE_EDGE_TOP             = 1,
    SB_DESKTOP_SURFACE_TOPLEVEL_RESIZE_EDGE_BOTTOM          = 2,
    SB_DESKTOP_SURFACE_TOPLEVEL_RESIZE_EDGE_LEFT            = 4,
    SB_DESKTOP_SURFACE_TOPLEVEL_RESIZE_EDGE_TOP_LEFT        = 5,
    SB_DESKTOP_SURFACE_TOPLEVEL_RESIZE_EDGE_BOTTOM_LEFT     = 6,
    SB_DESKTOP_SURFACE_TOPLEVEL_RESIZE_EDGE_RIGHT           = 8,
    SB_DESKTOP_SURFACE_TOPLEVEL_RESIZE_EDGE_TOP_RIGHT       = 9,
    SB_DESKTOP_SURFACE_TOPLEVEL_RESIZE_EDGE_BOTTOM_RIGHT    = 10,
} sb_desktop_surface_toplevel_resize_edge;

enum sb_desktop_surface_toplevel_state {
    SB_DESKTOP_SURFACE_TOPLEVEL_STATE_NORMAL            = 0,
    SB_DESKTOP_SURFACE_TOPLEVEL_STATE_MAXIMIZED         = 1,
    SB_DESKTOP_SURFACE_TOPLEVEL_STATE_FULLSCREEN        = 2,
    SB_DESKTOP_SURFACE_TOPLEVEL_STATE_RESIZING          = 4,
    SB_DESKTOP_SURFACE_TOPLEVEL_STATE_ACTIVATED         = 8,
    SB_DESKTOP_SURFACE_TOPLEVEL_STATE_TILED_LEFT        = 16,
    SB_DESKTOP_SURFACE_TOPLEVEL_STATE_TILED_RIGHT       = 32,
    SB_DESKTOP_SURFACE_TOPLEVEL_STATE_TILED_TOP         = 64,
    SB_DESKTOP_SURFACE_TOPLEVEL_STATE_TILED_BOTTOM      = 128,
    SB_DESKTOP_SURFACE_TOPLEVEL_STATE_SUSPENDED         = 256,
};

typedef enum sb_desktop_surface_toplevel_state
    sb_desktop_surface_toplevel_state_flags;

typedef struct sb_desktop_surface_t sb_desktop_surface_t;

SB_EXPORT
sb_desktop_surface_t* sb_desktop_surface_new(sb_desktop_surface_role role);

SB_EXPORT
void sb_desktop_surface_set_parent(sb_desktop_surface_t *desktop_surface,
                                   sb_desktop_surface_t *parent);

/// \brief Get the surface object of the desktop surface.
SB_EXPORT
sb_surface_t*
sb_desktop_surface_surface(sb_desktop_surface_t *desktop_surface);

SB_EXPORT
void sb_desktop_surface_show(sb_desktop_surface_t *desktop_surface);

SB_EXPORT
void sb_desktop_surface_hide(sb_desktop_surface_t *desktop_surface);

SB_EXPORT
sb_desktop_surface_toplevel_state_flags
sb_desktop_surface_toplevel_states(sb_desktop_surface_t *desktop_surface);

/// \brief Tell the geometry is the actual window area to the compositor.
///
/// A Wayland client is CSD (client side decoration). It means the window
/// should draw it's own decorations such as shadow.
/// The shadow may not treated as a part of the window.
///
/// The outside of this geometry may not appear your task switcher.
/// Some compositors offer the window magnet feature. This method give the
/// hint to the compositor how to magnet the windows each other.
SB_EXPORT
void sb_desktop_surface_set_wm_geometry(sb_desktop_surface_t *desktop_surface,
                                        const sb_rect_t *geometry);

const sb_size_t*
sb_desktop_surface_toplevel_minimum_size(sb_desktop_surface_t *desktop_surface);

/// \brief Set minimum size of the toplevel desktop surface.
///
/// This changes the field and call `_set_min_size` method in XDG toplevel.
void sb_desktop_surface_toplevel_set_minimum_size(
    sb_desktop_surface_t *desktop_surface, const sb_size_t *size);

/// \brief Close if the desktop surface has a toplevel role.
///
/// Close the last toplevel desktop surface may quit the application.
SB_EXPORT
void sb_desktop_surface_toplevel_close(sb_desktop_surface_t *desktop_surface);

/// \brief Start move the toplevel desktop surface.
SB_EXPORT
void sb_desktop_surface_toplevel_move(sb_desktop_surface_t *desktop_surface);

/// \brief Start resize the toplevel desktop surface with the given edge.
SB_EXPORT
void sb_desktop_surface_toplevel_resize(sb_desktop_surface_t *desktop_surface,
    sb_desktop_surface_toplevel_resize_edge edge);

/// \brief Maximize the window.
SB_EXPORT
void sb_desktop_surface_toplevel_set_maximized(
    sb_desktop_surface_t *desktop_surface);

/// \brief Restore the window from maximized state.
SB_EXPORT
void sb_desktop_surface_toplevel_unset_maximized(
    sb_desktop_surface_t *desktop_surface);

/// \brief Minimize the toplevel desktop surface.
SB_EXPORT
void sb_desktop_surface_toplevel_set_minimized(
    sb_desktop_surface_t *desktop_surface);

SB_EXPORT
void sb_desktop_surface_add_event_listener(
    sb_desktop_surface_t *desktop_surface,
    enum sb_event_type event_type,
    void (*listener)(sb_event_t*));

void sb_desktop_surface_on_resize(sb_desktop_surface_t *desktop_surface,
                                  sb_event_t *event);

void sb_desktop_surface_on_state_change(sb_desktop_surface_t *desktop_surface,
                                        sb_event_t *event);

#endif /* _FOUNDATION_DESKTOP_SURFACE_H */
