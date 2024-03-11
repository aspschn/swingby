#include <foundation/view.h>

#include <stdlib.h>

#include <foundation/surface.h>

struct ft_view_t {
    ft_surface_t *_surface;
    ft_rect_t _geometry;
    ft_view_t *_parent;
    ft_color_t _color;
};

ft_view_t* ft_view_new(ft_view_t *parent, const ft_rect_t *geometry)
{
    ft_view_t *view = malloc(sizeof(ft_view_t));

    view->_surface = NULL;
    view->_parent = parent;
    view->_geometry.pos = geometry->pos;
    view->_geometry.size = geometry->size;

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

void ft_view_set_color(ft_view_t *view, const ft_color_t *color)
{
    view->_color = *color;
}
