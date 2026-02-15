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
    shadow_color.r = 0.0f;
    shadow_color.g = 0.0f;
    shadow_color.b = 0.0f;
    shadow_color.a = 0.4f;
    sb_view_set_color(decoration->shadow.view, &shadow_color);

    // Resize.
    decoration->resize.thickness = 5.0f;
    decoration->resize.view = sb_view_new(decoration->view, &g);
    sb_color_t resize_color; // Must transparent. Green for debug.
    resize_color.r = 0.0f;
    resize_color.g = 1.0f;
    resize_color.b = 0.0f;
    resize_color.a = 1.0f;
    sb_view_set_color(decoration->resize.view, &resize_color);

    // Border.
    decoration->border.thickness = 1.0f;
    decoration->border.view = sb_view_new(decoration->view, &g);
    sb_color_t border_color;
    border_color.r = 0.0f;
    border_color.g = 0.0f;
    border_color.b = 0.0f;
    border_color.a = 1.0f;
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

sb_size_t decoration_border_size(struct decoration *decoration)
{
    const sb_rect_t *geometry = sb_view_geometry(decoration->border.view);

    return geometry->size;
}

void decoration_set_size(struct decoration *decoration, sb_size_t size)
{
    // Calculate the body size.
    sb_size_t body_size;
    body_size.width = size.width - (decoration->shadow.thickness * 2);
    body_size.height = size.height - (decoration->shadow.thickness * 2);
    body_size.height -= decoration->title_bar->height;

    // Update decoration root.
    sb_rect_t decoration_geometry;
    decoration_geometry.pos.x = 0.0f;
    decoration_geometry.pos.y = 0.0f;
    decoration_geometry.size = size;
    sb_view_set_geometry(decoration->view, &decoration_geometry);

    // Update shadow.
    sb_rect_t shadow_geometry;
    shadow_geometry.pos.x = 0.0f;
    shadow_geometry.pos.y = 0.0f;
    shadow_geometry.size = size;
    sb_view_set_geometry(decoration->shadow.view, &shadow_geometry);

    // Update resize.
    sb_rect_t resize_geometry;
    resize_geometry.pos.x =
        decoration->shadow.thickness - decoration->resize.thickness;
    resize_geometry.pos.y =
        decoration->shadow.thickness - decoration->resize.thickness;
    resize_geometry.size.width =
        body_size.width + (decoration->resize.thickness * 2);
    resize_geometry.size.height =
        body_size.height + (decoration->resize.thickness * 2);
    resize_geometry.size.height += decoration->title_bar->height;
    sb_view_set_geometry(decoration->resize.view, &resize_geometry);

    // Update border.
    sb_rect_t border_geometry;
    border_geometry.pos.x =
        window_decoration_border_offset(decoration->window);
    border_geometry.pos.y =
        window_decoration_border_offset(decoration->window);
    border_geometry.size.width =
        body_size.width + decoration->border.thickness * 2;
    border_geometry.size.height =
        body_size.height + decoration->border.thickness * 2;
    border_geometry.size.height += decoration->title_bar->height;
    sb_view_set_geometry(decoration->border.view, &border_geometry);

    // Update title bar.
    sb_rect_t title_bar_geometry;
    title_bar_geometry.pos.x =
        window_decoration_title_bar_offset(decoration->window);
    title_bar_geometry.pos.y =
        window_decoration_title_bar_offset(decoration->window);
    title_bar_geometry.size.width = body_size.width;
    title_bar_geometry.size.height = decoration->title_bar->height;
    title_bar_set_geometry(decoration->title_bar, title_bar_geometry);
}

void _decoration_set_size_nodeco(struct decoration *decoration, sb_size_t size)
{
    // TODO.
}
