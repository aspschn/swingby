#include <swingby/view.h>

#include <stdlib.h>

#include <swingby/log.h>
#include <swingby/surface.h>
#include <swingby/image.h>
#include <swingby/filter.h>
#include <swingby/list.h>
#include <swingby/event.h>

#include "helpers/shared.h"

struct sb_view_t {
    sb_surface_t *_surface;
    sb_rect_t _geometry;
    sb_view_t *_parent;
    /// \brief View's color if the view fill type is single color.
    sb_color_t _color;
    sb_list_t *_children;
    enum sb_view_fill_type fill_type;
    sb_image_t *image;
    /// \brief Rectangle view radius.
    sb_view_radius_t radius;
    /// \brief View effect filters.
    sb_list_t *filters;
    /// \brief Clip child views. Default is false.
    bool clip;
    enum sb_cursor_shape cursor_shape;
    sb_list_t *event_listeners;
};

//!<====================
//!< Helper Functions
//!<====================


//!<===========
//!< View
//!<===========

sb_view_t* sb_view_new(sb_view_t *parent, const sb_rect_t *geometry)
{
    sb_view_t *view = malloc(sizeof(sb_view_t));

    view->_surface = NULL;
    view->_parent = parent;
    sb_log_debug("sb_view_new() - view: %p, parent: %p\n", view, parent);
    view->_geometry.pos = geometry->pos;
    view->_geometry.size = geometry->size;
    view->_color.r = 255;
    view->_color.g = 255;
    view->_color.b = 255;
    view->_color.a = 255;

    // Set initial radius.
    sb_view_radius_t radius = { 0.0f, 0.0f, 0.0f, 0.0f };
    view->radius = radius;

    view->_children = sb_list_new();

    view->fill_type = SB_VIEW_FILL_TYPE_SINGLE_COLOR;
    view->image = NULL;

    view->filters = sb_list_new();

    view->event_listeners = sb_list_new();

    view->clip = false;

    view->cursor_shape = SB_CURSOR_SHAPE_DEFAULT;

    if (parent != NULL) {
        // Append the new view to the child list of the parent view.
        sb_list_push(parent->_children, (void*)view);
        // Inherit parent's surface.
        view->_surface = parent->_surface;
    }

    return view;
}

void sb_view_set_surface(sb_view_t *view, sb_surface_t *surface)
{
    view->_surface = surface;
}

sb_surface_t* sb_view_surface(const sb_view_t *view)
{
    return view->_surface;
}

const sb_rect_t* sb_view_geometry(const sb_view_t *view)
{
    return &view->_geometry;
}

void sb_view_set_geometry(sb_view_t *view, const sb_rect_t *geometry)
{
    // TODO: Equality check.
    view->_geometry = *geometry;

    if (view->_surface == NULL) {
        sb_log_warn("sb_view_set_geometry() - surface is NULL\n");
    }
    sb_surface_update(view->_surface);
}

const sb_color_t* sb_view_color(const sb_view_t *view)
{
    return &view->_color;
}

enum sb_view_fill_type sb_view_fill_type(const sb_view_t *view)
{
    return view->fill_type;
}

void sb_view_set_fill_type(sb_view_t *view, enum sb_view_fill_type fill_type)
{
    view->fill_type = fill_type;
}

sb_image_t* sb_view_image(sb_view_t *view)
{
    return view->image;
}

void sb_view_set_image(sb_view_t *view, sb_image_t *image)
{
    view->image = image;
}

const sb_view_radius_t* sb_view_radius(const sb_view_t *view)
{
    return &view->radius;
}

void sb_view_set_radius(sb_view_t *view, const sb_view_radius_t *radius)
{
    if (radius->top_left < 0.0f || radius->top_right < 0.0f ||
        radius->bottom_right < 0.0f || radius->bottom_left < 0.0f) {
        sb_log_warn("sb_view_set_radius() - Radius must greater than zero.\n");
    }
    view->radius = *radius;
}

void sb_view_add_filter(sb_view_t *view, const sb_filter_t *filter)
{
    sb_list_push(view->filters, (sb_filter_t*)filter);
}

const sb_list_t* sb_view_filters(const sb_view_t *view)
{
    return view->filters;
}

sb_list_t* sb_view_children(sb_view_t *view)
{
    return view->_children;
}

sb_view_t* sb_view_child_at(sb_view_t *view, const sb_point_t *position)
{
    sb_rect_t local_geo;
    local_geo.pos.x = 0;
    local_geo.pos.y = 0;
    local_geo.size = sb_view_geometry(view)->size;

    if (sb_rect_contains_point(&local_geo, position) == false) {
        return NULL;
    }

    for (int i = sb_list_length(view->_children); i > 0; --i) {
        sb_view_t *child = sb_list_at(view->_children, i - 1);
        if (sb_rect_contains_point((sb_rect_t*)sb_view_geometry(child),
            position)) {
            return child;
        }
    }

    return NULL;
}

sb_view_t* sb_view_parent(sb_view_t *view)
{
    return view->_parent;
}

void sb_view_set_color(sb_view_t *view, const sb_color_t *color)
{
    // TODO: Equality check.
    view->_color = *color;

    if (view->_surface == NULL) {
        sb_log_warn("sb_view_set_color() - surface is NULL.\n");
    }
    sb_surface_update(view->_surface);
}

bool sb_view_clip(const sb_view_t *view)
{
    return view->clip;
}

void sb_view_set_clip(sb_view_t *view, bool clip)
{
    view->clip = clip;
}

enum sb_cursor_shape sb_view_cursor_shape(const sb_view_t *view)
{
    return view->cursor_shape;
}

void sb_view_set_cursor_shape(sb_view_t *view, enum sb_cursor_shape shape)
{
    view->cursor_shape = shape;
}

void sb_view_add_event_listener(sb_view_t *view,
                                enum sb_event_type event_type,
                                void (*listener)(sb_event_t*))
{
    sb_event_listener_tuple_t *tuple = sb_event_listener_tuple_new(
        event_type, listener);
    sb_list_push(view->event_listeners, (void*)tuple);
}

//!<====================
//!< View Radius
//!<====================

bool sb_view_radius_is_zero(const sb_view_radius_t *radius)
{
    if (radius->top_left == 0.0f && radius->top_right == 0.0f &&
        radius->bottom_right == 0.0f && radius->bottom_left == 0.0f) {
        return true;
    }

    return false;
}

float sb_view_radius_top_left(const sb_view_radius_t *radius)
{
    return radius->top_left;
}

float sb_view_radius_top_right(const sb_view_radius_t *radius)
{
    return radius->top_right;
}

float sb_view_radius_bottom_right(const sb_view_radius_t *radius)
{
    return radius->bottom_right;
}

float sb_view_radius_bottom_left(const sb_view_radius_t *radius)
{
    return radius->bottom_left;
}

//!<====================
//!< Event Handlers
//!<====================

void sb_view_on_pointer_enter(sb_view_t *view, sb_event_t *event)
{
    _event_listener_filter_for_each(view->event_listeners,
        SB_EVENT_TYPE_POINTER_ENTER, event);
}

void sb_view_on_pointer_leave(sb_view_t *view, sb_event_t *event)
{
    _event_listener_filter_for_each(view->event_listeners,
        SB_EVENT_TYPE_POINTER_LEAVE, event);
}

void sb_view_on_pointer_move(sb_view_t *view, sb_event_t *event)
{
    // sb_log_debug("sb_view_on_pointer_move() - (%f, %f)\n", event->pointer.position.x, event->pointer.position.y);

    _event_listener_filter_for_each(view->event_listeners,
        SB_EVENT_TYPE_POINTER_MOVE, event);
}

void sb_view_on_pointer_press(sb_view_t *view, sb_event_t *event)
{
    sb_log_debug("sb_view_on_pointer_press() - (%.2f, %.2f) view: %p\n",
        event->pointer.position.x, event->pointer.position.y,
        event->target);

    _event_listener_filter_for_each(view->event_listeners,
        SB_EVENT_TYPE_POINTER_PRESS, event);
}

void sb_view_on_pointer_release(sb_view_t *view, sb_event_t *event)
{
    _event_listener_filter_for_each(view->event_listeners,
        SB_EVENT_TYPE_POINTER_RELEASE, event);
}

void sb_view_on_pointer_click(sb_view_t *view, sb_event_t *event)
{
    sb_log_debug("sb_view_on_pointer_click() - (%.2f, %.2f) view: %p\n",
        event->pointer.position.x, event->pointer.position.y,
        event->target);

    _event_listener_filter_for_each(view->event_listeners,
        SB_EVENT_TYPE_POINTER_CLICK, event);
}

void sb_view_on_pointer_double_click(sb_view_t *view, sb_event_t *event)
{
    _event_listener_filter_for_each(view->event_listeners,
        SB_EVENT_TYPE_POINTER_DOUBLE_CLICK, event);
}

void sb_view_on_pointer_scroll(sb_view_t *view, sb_event_t *event)
{
    _event_listener_filter_for_each(view->event_listeners,
        SB_EVENT_TYPE_POINTER_SCROLL, event);
}

