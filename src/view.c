#include <foundation/view.h>

#include <stdlib.h>

#include <foundation/log.h>
#include <foundation/surface.h>
#include <foundation/list.h>
#include <foundation/event.h>

struct ft_view_t {
    ft_surface_t *_surface;
    ft_rect_t _geometry;
    ft_view_t *_parent;
    ft_color_t _color;
    ft_list_t *_children;
};

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

    if (parent != NULL) {
        ft_list_push(parent->_children, (void*)view);
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

const ft_color_t* ft_view_color(ft_view_t *view)
{
    return &view->_color;
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
    view->_color = *color;
}

void ft_view_on_pointer_move(ft_view_t *view, ft_event_t *event)
{
    ft_log_debug("ft_view_on_pointer_move() - (%f, %f)\n", event->pointer.position.x, event->pointer.position.y);
}

void ft_view_on_pointer_press(ft_view_t *view, ft_event_t *event)
{
    ft_log_debug("ft_view_on_pointer_press() - (%.2f, %.2f)\n", event->pointer.position.x, event->pointer.position.y);
}

void ft_view_on_pointer_release(ft_view_t *view, ft_event_t *event)
{
    //
}

void ft_view_on_pointer_click(ft_view_t *view, ft_event_t *event)
{
    ft_log_debug("ft_view_on_pointer_click()\n");
}

