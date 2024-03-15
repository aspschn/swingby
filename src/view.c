#include <foundation/view.h>

#include <stdlib.h>

#include <foundation/surface.h>
#include <foundation/list.h>

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
    if (ft_rect_contains_point(&view->_geometry, position) == false) {
        return NULL;
    }

    for (int i = ft_list_length(view->_children); i > 0; --i) {
        ft_view_t *child = ft_list_at(view->_children, i - 1);
        if (ft_rect_contains_point(&child->_geometry, position)) {
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
