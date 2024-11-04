#include <swingby/view.h>

#include <stdlib.h>

#include <swingby/log.h>
#include <swingby/surface.h>
#include <swingby/image.h>
#include <swingby/filter.h>
#include <swingby/list.h>
#include <swingby/event.h>

struct sb_view_t {
    sb_surface_t *_surface;
    sb_rect_t _geometry;
    sb_view_t *_parent;
    sb_color_t _color;
    sb_list_t *_children;
    enum sb_view_fill_type fill_type;
    sb_image_t *image;
    sb_view_radius_t radius;
    sb_list_t *filters;
    sb_list_t *event_listeners;
};

//!<====================
//!< Helper Functions
//!<====================

static void _event_listener_filter_for_each(sb_list_t *listeners,
                                            enum sb_event_type type,
                                            sb_event_t *event)
{
    uint64_t length = sb_list_length(listeners);
    for (uint64_t i = 0; i < length; ++i) {
        sb_event_listener_tuple_t *tuple = sb_list_at(listeners, i);
        if (tuple->type == type) {
            tuple->listener(event);
        }
    }
}

//!<===========
//!< View
//!<===========

sb_view_t* sb_view_new(sb_view_t *parent, const sb_rect_t *geometry)
{
    sb_view_t *view = malloc(sizeof(sb_view_t));

    view->_surface = NULL;
    view->_parent = parent;
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

const sb_rect_t* sb_view_geometry(sb_view_t *view)
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

const sb_color_t* sb_view_color(sb_view_t *view)
{
    return &view->_color;
}

enum sb_view_fill_type sb_view_fill_type(sb_view_t *view)
{
    return view->fill_type;
}

void sb_view_set_fill_type(sb_view_t *view, enum sb_view_fill_type fill_type)
{
    // From single color to single color. Do nothing.
    if (view->fill_type == SB_VIEW_FILL_TYPE_SINGLE_COLOR &&
        fill_type == SB_VIEW_FILL_TYPE_SINGLE_COLOR) {
        return;
    }

    // From single color to image.
    if (view->fill_type == SB_VIEW_FILL_TYPE_SINGLE_COLOR &&
        fill_type == SB_VIEW_FILL_TYPE_IMAGE) {
        view->fill_type = SB_VIEW_FILL_TYPE_IMAGE;

        sb_size_i_t size;
        size.width = view->_geometry.size.width;
        size.height = view->_geometry.size.height;
        view->image = sb_image_new(&size, SB_IMAGE_FORMAT_RGBA32);

        return;
    }

    // From image to image. Do nothing.
    if (view->fill_type == SB_VIEW_FILL_TYPE_IMAGE &&
        fill_type == SB_VIEW_FILL_TYPE_IMAGE) {
        return;
    }

    // From image to single color.
    if (view->fill_type == SB_VIEW_FILL_TYPE_IMAGE &&
        fill_type == SB_VIEW_FILL_TYPE_SINGLE_COLOR) {
        view->fill_type = SB_VIEW_FILL_TYPE_SINGLE_COLOR;

        return;
    }
}

sb_image_t* sb_view_image(sb_view_t *view)
{
    return view->image;
}

const sb_view_radius_t* sb_view_radius(sb_view_t *view)
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
    sb_log_debug("sb_view_on_pointer_press() - (%.2f, %.2f)\n", event->pointer.position.x, event->pointer.position.y);

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
    sb_log_debug("sb_view_on_pointer_click()\n");

    _event_listener_filter_for_each(view->event_listeners,
        SB_EVENT_TYPE_POINTER_CLICK, event);
}

void sb_view_on_pointer_double_click(sb_view_t *view, sb_event_t *event)
{
    _event_listener_filter_for_each(view->event_listeners,
        SB_EVENT_TYPE_POINTER_DOUBLE_CLICK, event);
}

