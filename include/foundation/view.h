#ifndef _FOUNDATION_VIEW_H
#define _FOUNDATION_VIEW_H

#include <foundation/rect.h>
#include <foundation/surface.h>
#include <foundation/color.h>
#include <foundation/event.h>

typedef struct ft_list_t ft_list_t;
typedef struct ft_event_t ft_event_t;

typedef struct ft_view_t ft_view_t;

ft_view_t* ft_view_new(ft_view_t *parent, const ft_rect_t *geometry);

void ft_view_set_surface(ft_view_t *view, ft_surface_t *surface);

const ft_rect_t* ft_view_geometry(ft_view_t *view);

void ft_view_set_geometry(ft_view_t *view, const ft_rect_t *geometry);

const ft_color_t* ft_view_color(ft_view_t *view);

void ft_view_set_color(ft_view_t *view, const ft_color_t *color);

ft_list_t* ft_view_children(ft_view_t *view);

ft_view_t* ft_view_child_at(ft_view_t *view, const ft_point_t *position);

ft_view_t* ft_view_parent(ft_view_t *view);

void ft_view_add_event_listener(ft_view_t *view,
                                ft_event_type event_type,
                                void (*listener)(ft_event_t*));

//!<====================
//!< Event Handlers
//!<====================

void ft_view_on_pointer_enter(ft_view_t *view, ft_event_t *event);

void ft_view_on_pointer_leave(ft_view_t *view, ft_event_t *event);

void ft_view_on_pointer_move(ft_view_t *view, ft_event_t *event);

void ft_view_on_pointer_press(ft_view_t *view, ft_event_t *event);

void ft_view_on_pointer_click(ft_view_t *view, ft_event_t *event);

void ft_view_on_pointer_release(ft_view_t *view, ft_event_t *event);

#endif /* _FOUNDATION_VIEW_H */
