#include "window.h"

#include <stdlib.h>

sbgears_decoration* decoration_new(const sbgears_window *window)
{
    sbgears_decoration *decoration = malloc(sizeof(sbgears_decoration));

    // Surface.
    sb_surface_t *surface = sb_desktop_surface_surface(
        window->sb_desktop_surface);

    // Create border.
    sb_rect_t border_geometry = {
        .position = { .x = 0.0f, .y = 0.0f },
        .size = {
            .width = sb_surface_size(surface)->width,
            .height = sb_surface_size(surface)->height,
        },
    };
    sb_color_t border_color = { .r = 0.5f, .g = 0.5f, .b = 0.5f, .a = 1.0f };
    decoration->border = sb_view_new(window->root_view, &border_geometry);
    sb_view_set_color(decoration->border, &border_color);
    sb_view_set_cursor_shape(decoration->border, SB_CURSOR_SHAPE_NW_RESIZE);

    // Create title bar.
    sb_rect_t title_geometry = {
        .position = {
            .x = SBGEARS_WINDOW_BORDER_SIZE,
            .y = SBGEARS_WINDOW_BORDER_SIZE,
        },
        .size = {
            .width = border_geometry.size.width
                - (SBGEARS_WINDOW_BORDER_SIZE * 2),
            .height = SBGEARS_WINDOW_TITLE_BAR_SIZE,
        },
    };
    sb_color_t title_color = { .r = 0.3f, .g = 0.3f, .b = 1.0f, .a = 1.0f };
    // TODO: This break this demo. Swingby must patched for it.
    decoration->title_bar = sb_view_new(decoration->border, &title_geometry);
    sb_view_set_color(decoration->title_bar, &title_color);

    return decoration;
}

sbgears_window* window_new()
{
    sbgears_window *window = malloc(sizeof(sbgears_window));

    window->sb_desktop_surface = sb_desktop_surface_new(
        SB_DESKTOP_SURFACE_ROLE_TOPLEVEL);

    window->root_view = sb_surface_root_view(
        sb_desktop_surface_surface(window->sb_desktop_surface)
    );

    window->decoration = decoration_new(window);

    return window;
}

void window_set_size(sbgears_window *window, const sb_size_i_t *size)
{
    sb_surface_t *surface = sb_desktop_surface_surface(
        window->sb_desktop_surface);

    // Surface size.
    sb_size_t new_size = {
        .width = (float)size->width,
        .height = (float)size->height,
    };

    sb_surface_set_size(surface, &new_size);

    // Decoration geometries.
    sb_rect_t border_geometry = {
        .position = {
            .x = 0.0f,
            .y = 0.0f,
        },
        .size = {
            .width = new_size.width,
            .height = new_size.height,
         },
    };

    sb_view_set_geometry(window->decoration->border, &border_geometry);

    // Body geometry.
    sb_rect_t body_geometry = {
        .position = {
            .x = SBGEARS_WINDOW_BORDER_SIZE,
            .y = SBGEARS_WINDOW_BORDER_SIZE + SBGEARS_WINDOW_TITLE_BAR_SIZE,
        },
        .size = {
            .width = new_size.width - (SBGEARS_WINDOW_BORDER_SIZE * 2),
            .height = new_size.height - (SBGEARS_WINDOW_BORDER_SIZE * 2)
                - SBGEARS_WINDOW_TITLE_BAR_SIZE,
         },
    };

    sb_view_set_geometry(window->body, &body_geometry);
}

void window_free(sbgears_window *window)
{
    // TODO
}
