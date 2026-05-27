#include "window.h"

#include <stdlib.h>

static void border_on_press(sb_event_t *event, void *user_data)
{
    sbgears_window *window = user_data;

    event->propagation = false;

    sb_desktop_surface_toplevel_resize(window->sb_desktop_surface,
        SB_DESKTOP_SURFACE_TOPLEVEL_RESIZE_EDGE_BOTTOM_RIGHT);
}

static void title_bar_on_press(sb_event_t *event, void *user_data)
{
    sbgears_window *window = user_data;

    event->propagation = false;

    sb_desktop_surface_toplevel_move(window->sb_desktop_surface);
}

static void button_on_paint(sb_event_t *event, void *user_data)
{
    sb_view_t *button = event->target;

    sb_canvas_t *canvas = sb_view_canvas(button);
    sb_paint_t *paint = sb_canvas_paint(canvas);

    const sb_rect_t *src = sb_view_geometry(button);
    sb_rect_t rect = {
        .position = { .x = 0.0f, .y = 0.0f },
        .size = { .width = src->size.width, .height = src->size.height },
    };
    paint->fill_color = (sb_color_t){
        .r = 0.5f, .g = 0.5f, .b = 0.5f, .a = 1.0f
    };
    sb_canvas_draw_rect(canvas, &rect, paint);

    paint->stroke_color = (sb_color_t){
        .r = 0.0f, .g = 0.0f, .b = 0.0f, .a = 1.0f
    };
    paint->stroke_width = 1.0f;
    {
        sb_point_t p1 = { .x = 4.0f, .y = 4.0f };
        sb_point_t p2 = { .x = 18.0f, .y = 18.0f };
        sb_canvas_draw_line(canvas, &p1, &p2, paint);
    }
    {
        sb_point_t p1 = { .x = 18.0f, .y = 4.0f };
        sb_point_t p2 = { .x = 4.0f, .y = 18.0f };
        sb_canvas_draw_line(canvas, &p1, &p2, paint);
    }
}

static void button_on_press(sb_event_t *event, void *user_data)
{
    sbgears_window *window = user_data;

    event->propagation = false;
}

static void button_on_click(sb_event_t *event, void *user_data)
{
    sbgears_window *window = user_data;

    event->propagation = false;

    sb_desktop_surface_toplevel_close(window->sb_desktop_surface);
}

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

    sb_view_add_event_listener(decoration->border,
        SB_EVENT_TYPE_POINTER_PRESS,
        border_on_press,
        (void*)window);

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
    sb_color_t title_color = { .r = 0.3f, .g = 0.3f, .b = 0.65f, .a = 1.0f };
    // TODO: This break this demo. Swingby must patched for it.
    decoration->title_bar = sb_view_new(decoration->border, &title_geometry);
    sb_view_set_color(decoration->title_bar, &title_color);

    // Add events.
    sb_view_add_event_listener(decoration->title_bar,
        SB_EVENT_TYPE_POINTER_PRESS,
        title_bar_on_press,
        (void*)window);

    // Close button.
    sb_rect_t button_geometry;
    button_geometry.position = (sb_point_t){ .x = 5.0f, .y = 5.0f };
    button_geometry.size = (sb_size_t){ .width = 22.0f, .height = 22.0f };
    decoration->close_button = sb_view_new(decoration->title_bar,
        &button_geometry);
    sb_view_set_render_type(decoration->close_button,
        SB_VIEW_RENDER_TYPE_CANVAS);
    sb_view_add_event_listener(decoration->close_button,
        SB_EVENT_TYPE_PAINT,
        button_on_paint,
        NULL);

    sb_view_add_event_listener(decoration->close_button,
        SB_EVENT_TYPE_POINTER_PRESS,
        button_on_press,
        (void*)window);
    sb_view_add_event_listener(decoration->close_button,
        SB_EVENT_TYPE_POINTER_CLICK,
        button_on_click,
        (void*)window);

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

    //==========================
    // Decoration Geometries
    //==========================

    // Border.
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

    // Title bar.
    sb_rect_t border_rect = *sb_view_geometry(window->decoration->border);
    sb_rect_t title_bar_rect = *sb_view_geometry(window->decoration->title_bar);

    title_bar_rect.size.width = border_rect.size.width
        - (SBGEARS_WINDOW_BORDER_SIZE * 2);

    sb_view_set_geometry(window->decoration->title_bar, &title_bar_rect);

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
