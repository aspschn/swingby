#include <foundation/view.h>

#include <stdlib.h>

#include <foundation/log.h>
#include <foundation/surface.h>
#include <foundation/image.h>
#include <foundation/list.h>
#include <foundation/event.h>

struct ft_view_t {
    ft_surface_t *_surface;
    ft_rect_t _geometry;
    ft_view_t *_parent;
    ft_color_t _color;
    ft_list_t *_children;
    enum ft_view_fill_type fill_type;
    ft_image_t *image;
    ft_list_t *event_listeners;
};

//!<====================
//!< Helper Functions
//!<====================

static void _event_listener_filter_for_each(ft_list_t *listeners,
                                     ft_event_type type,
                                     ft_event_t *event)
{
    uint64_t length = ft_list_length(listeners);
    for (uint64_t i = 0; i < length; ++i) {
        ft_event_listener_tuple_t *tuple = ft_list_at(listeners, i);
        if (tuple->type == type) {
            tuple->listener(event);
        }
    }
}

//!<===========
//!< View
//!<===========

ft_view_t* ft_view_new(ft_view_t *parent, const ft_rect_t *geometry)
{
    ft_view_t *view = malloc(sizeof(ft_view_t));

    view->_surface = NULL;
    view->_parent = parent;
    view->_geometry.pos = geometry->pos;
    view->_geometry.size = geometry->size;
    view->_color.r = 255;
    view->_color.g = 255;
    view->_color.b = 255;
    view->_color.a = 255;

    view->_children = ft_list_new();

    view->fill_type = FT_VIEW_FILL_TYPE_SINGLE_COLOR;
    view->image = NULL;

    view->event_listeners = ft_list_new();

    if (parent != NULL) {
        // Append the new view to the child list of the parent view.
        ft_list_push(parent->_children, (void*)view);
        // Inherit parent's surface.
        view->_surface = parent->_surface;
    }

    return view;
}

void ft_view_set_surface(ft_view_t *view, ft_surface_t *surface)
{
    view->_surface = surface;
}

const ft_rect_t* ft_view_geometry(ft_view_t *view)
{
    return &view->_geometry;
}

void ft_view_set_geometry(ft_view_t *view, const ft_rect_t *geometry)
{
    // TODO: Equality check.
    view->_geometry = *geometry;

    ft_surface_update(view->_surface);
}

const ft_color_t* ft_view_color(ft_view_t *view)
{
    return &view->_color;
}

enum ft_view_fill_type ft_view_fill_type(ft_view_t *view)
{
    return view->fill_type;
}

void ft_view_set_fill_type(ft_view_t *view, enum ft_view_fill_type fill_type)
{
    // From single color to single color. Do nothing.
    if (view->fill_type == FT_VIEW_FILL_TYPE_SINGLE_COLOR &&
        fill_type == FT_VIEW_FILL_TYPE_SINGLE_COLOR) {
        return;
    }

    // From single color to image.
    if (view->fill_type == FT_VIEW_FILL_TYPE_SINGLE_COLOR &&
        fill_type == FT_VIEW_FILL_TYPE_IMAGE) {
        view->fill_type = FT_VIEW_FILL_TYPE_IMAGE;

        ft_size_i_t size;
        size.width = view->_geometry.size.width;
        size.height = view->_geometry.size.height;
        view->image = ft_image_new(&size, FT_IMAGE_FORMAT_RGBA32);

        return;
    }

    // From image to image. Do nothing.
    if (view->fill_type == FT_VIEW_FILL_TYPE_IMAGE &&
        fill_type == FT_VIEW_FILL_TYPE_IMAGE) {
        return;
    }

    // From image to single color.
    if (view->fill_type == FT_VIEW_FILL_TYPE_IMAGE &&
        fill_type == FT_VIEW_FILL_TYPE_SINGLE_COLOR) {
        view->fill_type = FT_VIEW_FILL_TYPE_SINGLE_COLOR;

        return;
    }
}

ft_image_t* ft_view_image(ft_view_t *view)
{
    return view->image;
}

ft_list_t* ft_view_children(ft_view_t *view)
{
    return view->_children;
}

ft_view_t* ft_view_child_at(ft_view_t *view, const ft_point_t *position)
{
    ft_rect_t local_geo;
    local_geo.pos.x = 0;
    local_geo.pos.y = 0;
    local_geo.size = ft_view_geometry(view)->size;

    if (ft_rect_contains_point(&local_geo, position) == false) {
        return NULL;
    }

    for (int i = ft_list_length(view->_children); i > 0; --i) {
        ft_view_t *child = ft_list_at(view->_children, i - 1);
        if (ft_rect_contains_point((ft_rect_t*)ft_view_geometry(child),
            position)) {
            return child;
        }
    }

    return NULL;
}

ft_view_t* ft_view_parent(ft_view_t *view)
{
    return view->_parent;
}

void ft_view_set_color(ft_view_t *view, const ft_color_t *color)
{
    // TODO: Equality check.
    view->_color = *color;

    ft_surface_update(view->_surface);
}

void ft_view_add_event_listener(ft_view_t *view,
                                ft_event_type event_type,
                                void (*listener)(ft_event_t*))
{
    ft_event_listener_tuple_t *tuple = ft_event_listener_tuple_new(
        event_type, listener);
    ft_list_push(view->event_listeners, (void*)tuple);
}

void ft_view_on_pointer_enter(ft_view_t *view, ft_event_t *event)
{
    _event_listener_filter_for_each(view->event_listeners,
        FT_EVENT_TYPE_POINTER_ENTER, event);
}

void ft_view_on_pointer_leave(ft_view_t *view, ft_event_t *event)
{
    _event_listener_filter_for_each(view->event_listeners,
        FT_EVENT_TYPE_POINTER_LEAVE, event);
}

void ft_view_on_pointer_move(ft_view_t *view, ft_event_t *event)
{
    // ft_log_debug("ft_view_on_pointer_move() - (%f, %f)\n", event->pointer.position.x, event->pointer.position.y);

    _event_listener_filter_for_each(view->event_listeners,
        FT_EVENT_TYPE_POINTER_MOVE, event);
}

void ft_view_on_pointer_press(ft_view_t *view, ft_event_t *event)
{
    ft_log_debug("ft_view_on_pointer_press() - (%.2f, %.2f)\n", event->pointer.position.x, event->pointer.position.y);

    _event_listener_filter_for_each(view->event_listeners,
        FT_EVENT_TYPE_POINTER_PRESS, event);
}

void ft_view_on_pointer_release(ft_view_t *view, ft_event_t *event)
{
    _event_listener_filter_for_each(view->event_listeners,
        FT_EVENT_TYPE_POINTER_RELEASE, event);
}

void ft_view_on_pointer_click(ft_view_t *view, ft_event_t *event)
{
    ft_log_debug("ft_view_on_pointer_click()\n");

    _event_listener_filter_for_each(view->event_listeners,
        FT_EVENT_TYPE_POINTER_CLICK, event);
}

