#ifndef _FOUNDATION_VIEW_H
#define _FOUNDATION_VIEW_H

#include <stdbool.h>

#include <swingby/common.h>
#include <swingby/rect.h>
#include <swingby/surface.h>
#include <swingby/color.h>
#include <swingby/cursor.h>
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

enum sb_view_render_type {
    SB_VIEW_RENDER_TYPE_SINGLE_COLOR,
    SB_VIEW_RENDER_TYPE_IMAGE,
    SB_VIEW_RENDER_TYPE_GLYPHS,
};

typedef struct sb_view_t sb_view_t;

/// \brief Create a new view with the parent and the geometry.
///
/// The parent must not be NULL. Pass a surface's root view for top level view.
/// Pass parent as NULL used in creation surface's root view.
SB_EXPORT
sb_view_t* sb_view_new(sb_view_t *parent, const sb_rect_t *geometry);

SB_EXPORT
void sb_view_set_surface(sb_view_t *view, sb_surface_t *surface);

/// \brief Get the surface which the view rely on.
SB_EXPORT
sb_surface_t* sb_view_surface(const sb_view_t *view);

/// \brief Get the geometry of the view.
SB_EXPORT
const sb_rect_t* sb_view_geometry(const sb_view_t *view);

/// \brief Set the geometry of the view.
SB_EXPORT
void sb_view_set_geometry(sb_view_t *view, const sb_rect_t *geometry);

/// \brief Get the color of the view if view's render type is single color.
SB_EXPORT
const sb_color_t* sb_view_color(const sb_view_t *view);

/// \brief Set the color of the view. Only for single color fill type view.
SB_EXPORT
void sb_view_set_color(sb_view_t *view, const sb_color_t *color);

/// \brief Get the render type of the view.
SB_EXPORT
enum sb_view_render_type sb_view_render_type(const sb_view_t *view);

/// \brief Set the render type of the view.
SB_EXPORT
void sb_view_set_render_type(sb_view_t *view, enum sb_view_render_type type);

/// \brief Get the image of the view. Returns NULL if there is no image.
SB_EXPORT
sb_image_t* sb_view_image(sb_view_t *view);

/// \brief Set the image of the view. Pass NULL to unset.
SB_EXPORT
void sb_view_set_image(sb_view_t *view, sb_image_t *image);

/// \brief Get the radius of the view.
SB_EXPORT
const sb_view_radius_t* sb_view_radius(const sb_view_t *view);

/// \brief Set the radius of the view.
SB_EXPORT
void sb_view_set_radius(sb_view_t *view, const sb_view_radius_t *radius);

/// \brief Add a filter to the view.
SB_EXPORT
void sb_view_add_filter(sb_view_t *view, const sb_filter_t *filter);

/// \brief Get the list of filters of the view.
SB_EXPORT
const sb_list_t* sb_view_filters(const sb_view_t *view);

/// \brief Get the clip property of the view.
SB_EXPORT
bool sb_view_clip(const sb_view_t *view);

/// \brief Set the clip property of the view.
SB_EXPORT
void sb_view_set_clip(sb_view_t *view, bool clip);

SB_EXPORT
enum sb_cursor_shape sb_view_cursor_shape(const sb_view_t *view);

SB_EXPORT
void sb_view_set_cursor_shape(sb_view_t *view, enum sb_cursor_shape shape);

/// \brief Get the list of the view's children.
sb_list_t* sb_view_children(sb_view_t *view);

/// \brief Get the child of the given position.
sb_view_t* sb_view_child_at(sb_view_t *view, const sb_point_t *position);

/// \brief Returns the parent view.
sb_view_t* sb_view_parent(sb_view_t *view);

sb_view_t* sb_view_remove_child(sb_view_t *view, sb_view_t *child);

sb_point_t sb_view_absolute_position(const sb_view_t *view);

SB_EXPORT
void sb_view_add_event_listener(sb_view_t *view,
                                enum sb_event_type event_type,
                                sb_event_listener_t listener,
                                void *user_data);

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

void sb_view_on_pointer_scroll(sb_view_t *view, sb_event_t *event);

void sb_view_on_move(sb_view_t *view, sb_event_t *event);

void sb_view_on_resize(sb_view_t *view, sb_event_t *event);

void sb_view_on_keyboard_key_press(sb_view_t *view, sb_event_t *event);

void sb_view_on_keyboard_key_release(sb_view_t *view, sb_event_t *event);

void sb_view_on_text_input(sb_view_t *view, sb_event_t *event);

#ifdef __cplusplus
}
#endif

#endif /* _FOUNDATION_VIEW_H */
