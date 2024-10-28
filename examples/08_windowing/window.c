#include "window.h"

#include <stdlib.h>

struct window* window_new(sb_size_t size)
{
    struct window *window = malloc(sizeof(struct window));

    window->desktop_surface =
        sb_desktop_surface_new(SB_DESKTOP_SURFACE_ROLE_TOPLEVEL);

    sb_surface_t *surface = sb_desktop_surface_surface(window->desktop_surface);
    sb_size_t initial_size;
    initial_size.width = size.width;
    initial_size.height = size.height;

    sb_surface_set_size(surface, &initial_size);

    // Set root view's color as transparent.
    sb_color_t trans;
    trans.r = 0;
    trans.g = 0;
    trans.b = 0;
    trans.a = 0;
    sb_view_set_color(sb_surface_root_view(surface), &trans);

    // Create decoration.
    window->decoration = decoration_new(window);
    title_bar_set_window(window->decoration->title_bar, window);

    // Create body.
    sb_rect_t body_geometry;
    body_geometry.size.width = size.width;
    body_geometry.size.height = size.height;
    window->body = sb_view_new(sb_surface_root_view(surface), &body_geometry);

    // Set body geometry.
    body_geometry = window_body_geometry(window);
    sb_view_set_geometry(window->body, &body_geometry);

    // Update initial decoration geometry.
    decoration_update_size(window->decoration);

    // Set window surface size.
    sb_size_t surface_size = window_whole_size(window);
    sb_surface_set_size(surface, &surface_size);

    // Window frame geometry hint.
    sb_rect_t frame_geometry = window_frame_geometry(window);
    /*
    sb_desktop_surface_set_wm_geometry(window->desktop_surface,
                                       &frame_geometry);
    */

    return window;
}

//!<============
//!< Sizes
//!<============

sb_size_t window_body_size(struct window *window)
{
    const sb_rect_t *geometry = sb_view_geometry(window->body);

    return geometry->size;
}

sb_size_t window_frame_size(struct window *window)
{
    sb_size_t body_size = window_body_size(window);

    sb_size_t size;
    size.width = body_size.width + (window->decoration->border.thickness * 2);
    size.height = body_size.height + (window->decoration->border.thickness * 2);
    // Add title bar height.
    size.height += window->decoration->title_bar->height;

    return size;
}

sb_size_t window_whole_size(struct window *window)
{
    sb_size_t size = decoration_size(window->decoration, window);

    return size;
}

//!<============
//!< Geometry
//!<============

sb_rect_t window_body_geometry(struct window *window)
{
    sb_rect_t geometry;
    geometry.size = window_body_size(window);
    geometry.pos.x = window->decoration->shadow.thickness;
    geometry.pos.y = window->decoration->shadow.thickness;
    geometry.pos.y += window->decoration->title_bar->height;

    return geometry;
}

sb_rect_t window_frame_geometry(struct window *window)
{
    float shadow_thickness = window->decoration->shadow.thickness;
    float border_thickness = window->decoration->border.thickness;

    sb_rect_t geometry;
    geometry.size = window_frame_size(window);
    geometry.pos.x = shadow_thickness - border_thickness;
    geometry.pos.y = shadow_thickness - border_thickness;

    return geometry;
}

