#ifndef _FOUNDATION_VIEW_H
#define _FOUNDATION_VIEW_H

#include <foundation/rect.h>
#include <foundation/surface.h>
#include <foundation/color.h>

typedef struct ft_list_t ft_list_t;

typedef struct ft_view_t ft_view_t;

ft_view_t* ft_view_new(ft_view_t *parent, const ft_rect_t *geometry);

void ft_view_set_surface(ft_view_t *view, ft_surface_t *surface);

const ft_rect_t* ft_view_geometry(ft_view_t *view);

const ft_color_t* ft_view_color(ft_view_t *view);

ft_list_t* ft_view_children(ft_view_t *view);

ft_view_t* ft_view_child_at(ft_view_t *view, const ft_point_t *position);

void ft_view_set_color(ft_view_t *view, const ft_color_t *color);

#endif /* _FOUNDATION_VIEW_H */
