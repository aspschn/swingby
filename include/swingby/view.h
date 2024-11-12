#ifndef _FOUNDATION_VIEW_H
#define _FOUNDATION_VIEW_H

#include <swingby/rect.h>
#include <swingby/surface.h>
#include <swingby/color.h>
#include <swingby/event.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct sb_image_t sb_image_t;
typedef struct sb_list_t sb_list_t;
typedef struct sb_filter_t sb_filter_t;
typedef struct sb_event_t sb_event_t;

typedef struct sb_view_radius_t {
    float top_left;
    float top_right;
    float bottom_right;
    float bottom_left;
} sb_view_radius_t;

enum sb_view_fill_type {
    SB_VIEW_FILL_TYPE_SINGLE_COLOR,
    SB_VIEW_FILL_TYPE_IMAGE,
};

typedef struct sb_view_t sb_view_t;

/// \brief Create a new view with the parent and the geometry.
///
/// The parent must not be NULL. Pass a surface's root view for top level view.
/// Pass parent as NULL used in creation surface's root view.
sb_view_t* sb_view_new(sb_view_t *parent, const sb_rect_t *geometry);

void sb_view_set_surface(sb_view_t *view, sb_surface_t *surface);

/// \brief Get the surface which the view rely on.
sb_surface_t* sb_view_surface(const sb_view_t *view);

/// \brief Get the geometry of the view.
const sb_rect_t* sb_view_geometry(const sb_view_t *view);

/// \brief Set the geometry of the view.
void sb_view_set_geometry(sb_view_t *view, const sb_rect_t *geometry);

/// \brief Get the color of the view if view's fill type is single color.
const sb_color_t* sb_view_color(const sb_view_t *view);

/// \brief Set the color of the view. Only for single color fill type view.
void sb_view_set_color(sb_view_t *view, const sb_color_t *color);

/// \brief Get the fill type of the view.
enum sb_view_fill_type sb_view_fill_type(const sb_view_t *view);

/// \brief Set the fill type of the view.
void sb_view_set_fill_type(sb_view_t *view, enum sb_view_fill_type fill_type);

/// \brief Get the image of the view. Valid if only fill type is image.
sb_image_t* sb_view_image(sb_view_t *view);

/// \brief Get the radius of the view.
const sb_view_radius_t* sb_view_radius(const sb_view_t *view);

/// \brief Set the radius of the view.
void sb_view_set_radius(sb_view_t *view, const sb_view_radius_t *radius);

/// \brief Add a filter to the view.
void sb_view_add_filter(sb_view_t *view, const sb_filter_t *filter);

/// \brief Get the list of filters of the view.
const sb_list_t* sb_view_filters(const sb_view_t *view);

/// \brief Get the list of the view's children.
sb_list_t* sb_view_children(sb_view_t *view);

/// \brief Get the child of the given position.
sb_view_t* sb_view_child_at(sb_view_t *view, const sb_point_t *position);

/// \brief Returns the parent view.
sb_view_t* sb_view_parent(sb_view_t *view);

void sb_view_add_event_listener(sb_view_t *view,
                                enum sb_event_type event_type,
                                void (*listener)(sb_event_t*));

//!<====================
//!< View Radius
//!<====================

bool sb_view_radius_is_zero(const sb_view_radius_t *radius);

float sb_view_radius_top_left(const sb_view_radius_t *radius);

float sb_view_radius_top_right(const sb_view_radius_t *radius);

float sb_view_radius_bottom_right(const sb_view_radius_t *radius);

float sb_view_radius_bottom_left(const sb_view_radius_t *radius);

//!<====================
//!< Event Handlers
//!<====================

void sb_view_on_pointer_enter(sb_view_t *view, sb_event_t *event);

void sb_view_on_pointer_leave(sb_view_t *view, sb_event_t *event);

void sb_view_on_pointer_move(sb_view_t *view, sb_event_t *event);

void sb_view_on_pointer_press(sb_view_t *view, sb_event_t *event);

void sb_view_on_pointer_click(sb_view_t *view, sb_event_t *event);

void sb_view_on_pointer_double_click(sb_view_t *view, sb_event_t *event);

void sb_view_on_pointer_release(sb_view_t *view, sb_event_t *event);

#ifdef __cplusplus
}
#endif

#endif /* _FOUNDATION_VIEW_H */
