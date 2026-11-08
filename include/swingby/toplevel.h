#ifndef SWINGBY_TOPLEVEL_H
#define SWINGBY_TOPLEVEL_H

#include <stdbool.h>

#include <swingby/common.h>
#include <swingby/event.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef struct sb_size_i_t sb_size_i_t;
typedef struct sb_rect_i_t sb_rect_i_t;
typedef struct sb_size_t sb_size_t;
typedef struct sb_rect_t sb_rect_t;
typedef struct sb_surface_t sb_surface_t;

enum sb_toplevel_resize_edge {
    SB_TOPLEVEL_RESIZE_EDGE_NONE            = 0,
    SB_TOPLEVEL_RESIZE_EDGE_TOP             = 1,
    SB_TOPLEVEL_RESIZE_EDGE_BOTTOM          = 2,
    SB_TOPLEVEL_RESIZE_EDGE_LEFT            = 4,
    SB_TOPLEVEL_RESIZE_EDGE_TOP_LEFT        = 5,
    SB_TOPLEVEL_RESIZE_EDGE_BOTTOM_LEFT     = 6,
    SB_TOPLEVEL_RESIZE_EDGE_RIGHT           = 8,
    SB_TOPLEVEL_RESIZE_EDGE_TOP_RIGHT       = 9,
    SB_TOPLEVEL_RESIZE_EDGE_BOTTOM_RIGHT    = 10,
};

enum sb_toplevel_state {
    SB_TOPLEVEL_STATE_NORMAL            = 0,
    SB_TOPLEVEL_STATE_MAXIMIZED         = 1,
    SB_TOPLEVEL_STATE_FULLSCREEN        = 2,
    SB_TOPLEVEL_STATE_RESIZING          = 4,
    SB_TOPLEVEL_STATE_ACTIVATED         = 8,
    SB_TOPLEVEL_STATE_TILED_LEFT        = 16,
    SB_TOPLEVEL_STATE_TILED_RIGHT       = 32,
    SB_TOPLEVEL_STATE_TILED_TOP         = 64,
    SB_TOPLEVEL_STATE_TILED_BOTTOM      = 128,
    SB_TOPLEVEL_STATE_SUSPENDED         = 256,
};

typedef enum sb_toplevel_state
    sb_toplevel_state_flags;

/// \struct sb_toplevel_t
/// \brief A toplevel surface.
typedef struct sb_toplevel_t sb_toplevel_t;

SB_EXPORT
sb_toplevel_t* sb_toplevel_new();

SB_EXPORT
void sb_toplevel_set_parent(sb_toplevel_t *toplevel,
                            sb_toplevel_t *parent);

/// \memberof sb_toplevel_t
/// \brief Get the `sb_surface_t` object of the desktop surface.
SB_EXPORT
sb_surface_t* sb_toplevel_surface(sb_toplevel_t *toplevel);

SB_EXPORT
void sb_toplevel_show(sb_toplevel_t *toplevel);

SB_EXPORT
void sb_toplevel_hide(sb_toplevel_t *toplevel);

SB_EXPORT
sb_toplevel_state_flags sb_toplevel_states(sb_toplevel_t *toplevel);

/// \memberof sb_toplevel_t
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
void sb_toplevel_set_wm_geometry(sb_toplevel_t *toplevel,
                                 sb_rect_i_t geometry);

sb_size_i_t sb_toplevel_minimum_size(sb_toplevel_t *toplevel);

/// \memberof sb_toplevel_t
/// \brief Set minimum size of the toplevel desktop surface.
///
/// This changes the field and call `_set_min_size` method in XDG toplevel.
void sb_toplevel_set_minimum_size(sb_toplevel_t *toplevel, sb_size_i_t size);

/// \memberof sb_toplevel_t
/// \brief Close the toplevel surface.
///
/// Close the last toplevel surface may quit the application.
SB_EXPORT
void sb_toplevel_close(sb_toplevel_t *toplevel);

/// \memberof sb_toplevel_t
/// \brief Start move the toplevel surface.
SB_EXPORT
void sb_toplevel_move(sb_toplevel_t *toplevel);

/// \memberof sb_toplevel_t
/// \brief Start resize the toplevel surface with the given edge.
SB_EXPORT
void sb_toplevel_resize(sb_toplevel_t *toplevel,
                        enum sb_toplevel_resize_edge edge);

/// \memberof sb_toplevel_t
/// \brief Maximize the toplevel surface.
SB_EXPORT
void sb_toplevel_set_maximized(sb_toplevel_t *toplevel);

/// \memberof sb_toplevel_t
/// \brief Restore the toplevel surface from maximized state.
SB_EXPORT
void sb_toplevel_unset_maximized(sb_toplevel_t *toplevel);

/// \memberof sb_toplevel_t
/// \brief Minimize the toplevel surface.
SB_EXPORT
void sb_toplevel_set_minimized(sb_toplevel_t *toplevel);

/// \memberof sb_toplevel_t
/// \brief Set a title hint for the compositior.
SB_EXPORT
void sb_toplevel_set_title(sb_toplevel_t *toplevel, const char *title);

/// \memberof sb_toplevel_t
/// \brief Set an application id hint for the compositor.
SB_EXPORT
void sb_toplevel_set_app_id(sb_toplevel_t *toplevel, const char *app_id);

/// \memberof sb_toplevel_t
/// \brief Open a window menu from the compositor.
SB_EXPORT
void sb_toplevel_show_window_menu(sb_toplevel_t *toplevel,
                                  sb_point_i_t position);

struct xdg_surface* sb_toplevel_xdg_surface(const sb_toplevel_t *toplevel);

void sb_toplevel_free(sb_toplevel_t *toplevel);

SB_EXPORT
void sb_toplevel_add_event_listener(sb_toplevel_t *toplevel,
                                    enum sb_event_type event_type,
                                    sb_event_listener_t listener,
                                    void *user_data);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* SWINGBY_TOPLEVEL_H */
