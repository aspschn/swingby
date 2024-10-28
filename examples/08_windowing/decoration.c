#include "decoration.h"

#include <stdlib.h>

#include "window.h"

struct decoration* decoration_new(struct window *window)
{
    struct decoration *decoration = malloc(sizeof(struct decoration));
    decoration->window = window;

    sb_surface_t *window_surface =
        sb_desktop_surface_surface(window->desktop_surface);

    sb_rect_t geometry;
    geometry.pos.x = 0;
    geometry.pos.y = 0;
    geometry.size.width = 200;
    geometry.size.height = 200;
    decoration->view = sb_view_new(
        sb_surface_root_view(window_surface),
        &geometry
    );

    // Transparent the view.
    sb_color_t trans;
    trans.r = 0;
    trans.g = 0;
    trans.b = 0;
    trans.a = 0;
    sb_view_set_color(decoration->view, &trans);

    // Initial geometry. Not important.
    sb_rect_t g;

    // Shadow.
    decoration->shadow.thickness = 40.0f;
    sb_rect_t shadow_geometry;
    shadow_geometry.pos.x = 0.0f;
    shadow_geometry.pos.y = 0.0f;
    shadow_geometry.size.width = 200.0f;
    shadow_geometry.size.height = 200.0f;
    decoration->shadow.view = sb_view_new(decoration->view, &shadow_geometry);
    sb_color_t shadow_color;
    shadow_color.r = 0;
    shadow_color.g = 0;
    shadow_color.b = 0;
    shadow_color.a = 100;
    sb_view_set_color(decoration->shadow.view, &shadow_color);

    // Resize.
    decoration->resize.thickness = 5.0f;
    decoration->resize.view = sb_view_new(decoration->view, &g);

    // Border.
    decoration->border.thickness = 1.0f;
    decoration->border.view = sb_view_new(decoration->view, &g);
    sb_color_t border_color;
    border_color.r = 0;
    border_color.g = 0;
    border_color.b = 0;
    border_color.a = 255;
    sb_view_set_color(decoration->border.view, &border_color);

    // Title bar.
    decoration->title_bar = title_bar_new(decoration->view);

    return decoration;
}

sb_size_t decoration_size(struct decoration *decoration, struct window *window)
{
    sb_size_t size;

    sb_size_t body_size = window_body_size(window);
    // Left and right shadow.
    size.width = body_size.width + decoration->shadow.thickness * 2;
    // Top and bottom shadow.
    size.height = body_size.height + decoration->shadow.thickness * 2;
    // Title bar height.
    size.height = size.height + TITLE_BAR_HEIGHT;

    return size;
}

void decoration_update_size(struct decoration *decoration)
{
    sb_size_t surface_size = window_whole_size(decoration->window);
    sb_size_t body_size = window_body_size(decoration->window);

    // Update shadow.
    sb_rect_t shadow_geometry;
    shadow_geometry.pos.x = 0.0f;
    shadow_geometry.pos.y = 0.0f;
    shadow_geometry.size = surface_size;
    sb_view_set_geometry(decoration->shadow.view, &shadow_geometry);

    // Update resize.

    // Update border.
    sb_rect_t border_geometry;
    border_geometry.pos.x =
        decoration->shadow.thickness - decoration->border.thickness;
    border_geometry.pos.y =
        decoration->shadow.thickness - decoration->border.thickness;
    border_geometry.size.width =
        body_size.width + decoration->border.thickness * 2;
    border_geometry.size.height =
        body_size.height + decoration->border.thickness * 2;
    border_geometry.size.height += decoration->title_bar->height;
    sb_view_set_geometry(decoration->border.view, &border_geometry);

    // Update title bar.
    sb_rect_t title_bar_geometry;
    title_bar_geometry.pos.x =
        decoration->shadow.thickness;
    title_bar_geometry.pos.y =
        decoration->shadow.thickness;
    title_bar_geometry.size.width = body_size.width;
    title_bar_set_geometry(decoration->title_bar, title_bar_geometry);
}
