#include <stdio.h>

#include <foundation/foundation.h>

ft_view_t *image_view = NULL;

#define WINDOW_SHADOW_THICKNESS     40
#define WINDOW_RESIZE_THICKNESS     5
#define WINDOW_BORDER_THICKNESS     1
#define WINDOW_TITLE_BAR_HEIGHT     30
#define WINDOW_DEFAULT_WIDTH        200
#define WINDOW_DEFAULT_HEIGHT       200

struct window {
    ft_desktop_surface_t *desktop_surface;
    ft_view_t *shadow;
    ft_view_t *resize;
    ft_view_t *border;
    ft_view_t *title_bar;
    ft_view_t *body;
};

struct window *window_global = NULL;

// `window_size` is from desktop surface resize event.
ft_size_t get_surface_size(ft_size_t *window_size)
{
    ft_size_t size;
    size.width = window_size->width + (WINDOW_SHADOW_THICKNESS * 2);
    size.height = window_size->height + (WINDOW_SHADOW_THICKNESS * 2);

    return size;
}

// Desktop surface wm_geometry.
ft_rect_t get_window_geometry(struct window *window)
{
    ft_surface_t *surface = ft_desktop_surface_surface(window->desktop_surface);
    const ft_size_t *surface_size = ft_surface_size(surface);

    ft_rect_t geometry;
    geometry.pos.x = WINDOW_SHADOW_THICKNESS - WINDOW_BORDER_THICKNESS;
    geometry.pos.y = WINDOW_SHADOW_THICKNESS - WINDOW_BORDER_THICKNESS;
    geometry.size.width = surface_size->width
        - (WINDOW_SHADOW_THICKNESS * 2)
        + (WINDOW_BORDER_THICKNESS * 2);
    geometry.size.height = surface_size->height
        - (WINDOW_SHADOW_THICKNESS * 2)
        + (WINDOW_BORDER_THICKNESS * 2);

    return geometry;
}

ft_rect_t get_shadow_geometry(struct window *window)
{
    ft_surface_t *surface = ft_desktop_surface_surface(window->desktop_surface);
    const ft_size_t *surface_size = ft_surface_size(surface);

    ft_rect_t geometry;
    geometry.pos.x = 0;
    geometry.pos.y = 0;
    geometry.size.width = surface_size->width;
    geometry.size.height = surface_size->height;

    return geometry;
}

ft_rect_t get_resize_geometry(struct window *window)
{
    const ft_size_t shadow_size = ft_view_geometry(window->shadow)->size;

    ft_rect_t geometry;
    geometry.pos.x = WINDOW_SHADOW_THICKNESS - WINDOW_RESIZE_THICKNESS;
    geometry.pos.y = WINDOW_SHADOW_THICKNESS - WINDOW_RESIZE_THICKNESS;
    geometry.size.width = shadow_size.width
        - (WINDOW_SHADOW_THICKNESS * 2)
        + (WINDOW_RESIZE_THICKNESS * 2);
    geometry.size.height = shadow_size.height
        - (WINDOW_SHADOW_THICKNESS * 2)
        + (WINDOW_RESIZE_THICKNESS * 2);

    return geometry;
}

ft_rect_t get_border_geometry(struct window *window)
{
    const ft_size_t resize_size = ft_view_geometry(window->resize)->size;

    ft_rect_t geometry;
    geometry.pos.x = WINDOW_RESIZE_THICKNESS - WINDOW_BORDER_THICKNESS;
    geometry.pos.y = WINDOW_RESIZE_THICKNESS - WINDOW_BORDER_THICKNESS;
    geometry.size.width = resize_size.width
        - (WINDOW_RESIZE_THICKNESS * 2)
        + (WINDOW_BORDER_THICKNESS * 2);
    geometry.size.height = resize_size.height
        - (WINDOW_RESIZE_THICKNESS * 2)
        + (WINDOW_BORDER_THICKNESS * 2);

    return geometry;
}

ft_rect_t get_title_bar_geometry(struct window *window)
{
    const ft_size_t border_size = ft_view_geometry(window->border)->size;

    ft_rect_t geometry;
    geometry.pos.x = WINDOW_BORDER_THICKNESS;
    geometry.pos.y = WINDOW_BORDER_THICKNESS;
    geometry.size.width = border_size.width - (WINDOW_BORDER_THICKNESS * 2);
    geometry.size.height = WINDOW_TITLE_BAR_HEIGHT;

    return geometry;
}

ft_rect_t get_body_geometry(struct window *window)
{
    const ft_size_t border_size = ft_view_geometry(window->border)->size;

    ft_rect_t geometry;
    geometry.pos.x = WINDOW_BORDER_THICKNESS;
    geometry.pos.y = WINDOW_BORDER_THICKNESS + WINDOW_TITLE_BAR_HEIGHT;
    geometry.size.width = border_size.width - (WINDOW_BORDER_THICKNESS * 2);
    geometry.size.height = border_size.height - (WINDOW_BORDER_THICKNESS * 2)
        - WINDOW_TITLE_BAR_HEIGHT;

    return geometry;
}


static void on_desktop_surface_resize(ft_event_t *event)
{
    // Resize the surface.
    ft_surface_t *surface = ft_desktop_surface_surface(
        window_global->desktop_surface);
    ft_rect_t new_geo;
    new_geo.size = get_surface_size(&event->resize.size);

    ft_surface_set_size(surface, &new_geo.size);

    ft_rect_t wm_geo;
    wm_geo = get_window_geometry(window_global);
    ft_desktop_surface_set_wm_geometry(window_global->desktop_surface, &wm_geo);
}

static void on_desktop_surface_state_change(ft_event_t *event)
{
    fprintf(stderr, "State changed. %.2fx%.2f\n",
            event->state_change.size.width,
            event->state_change.size.height);
    ft_surface_t *surface = ft_desktop_surface_surface(
        window_global->desktop_surface);

    int state = event->state_change.state;
    if (state == FT_DESKTOP_SURFACE_TOPLEVEL_STATE_MAXIMIZED) {
        ft_size_t size;
        size.width = event->state_change.size.width;
        size.height = event->state_change.size.height;
        ft_surface_set_size(surface, &size);
    }
}

static void on_surface_resize(ft_event_t *event)
{
    fprintf(stderr, "on_resize: %dx%d\n", (int)event->resize.size.width, (int)event->resize.size.height);

    // Update shadow size.
    ft_rect_t shadow_geometry = get_shadow_geometry(window_global);
    ft_view_set_geometry(window_global->shadow, &shadow_geometry);

    // Update resize size.
    ft_rect_t resize_geometry = get_resize_geometry(window_global);
    ft_view_set_geometry(window_global->resize, &resize_geometry);

    // Update border size.
    ft_rect_t border_geometry = get_border_geometry(window_global);
    ft_view_set_geometry(window_global->border, &border_geometry);

    // Update title bar size.
    ft_rect_t title_bar_geometry = get_title_bar_geometry(window_global);
    ft_view_set_geometry(window_global->title_bar, &title_bar_geometry);

    // Update body size.
    ft_rect_t body_geometry = get_body_geometry(window_global);
    ft_view_set_geometry(window_global->body, &body_geometry);
}

static void on_close_button_press(ft_event_t *event)
{
    // Stop event propagation.
    event->propagation = false;
}

static void on_close_button_click(ft_event_t *event)
{
    fprintf(stderr, "Close button clicked.\n");
    ft_desktop_surface_toplevel_close(window_global->desktop_surface);
}

static void on_title_bar_press(ft_event_t *event)
{
    ft_desktop_surface_toplevel_move(window_global->desktop_surface);
}


static void init_window(struct window *window)
{
    ft_color_t color;
    ft_size_t size;
    ft_rect_t geometry;
    ft_surface_t *surface = ft_desktop_surface_surface(window->desktop_surface);

    // Set the total surface size.
    size.width = WINDOW_DEFAULT_WIDTH + (WINDOW_SHADOW_THICKNESS * 2);
    size.height = WINDOW_DEFAULT_HEIGHT + (WINDOW_SHADOW_THICKNESS * 2);
    ft_surface_set_size(surface, &size);

    // Transparent root view.
    color.r = 0;
    color.g = 0;
    color.b = 0;
    color.a = 0;
    ft_view_set_color(ft_surface_root_view(surface), &color);

    // Set shadow.
    ft_rect_t shadow_geometry = get_shadow_geometry(window);
    window->shadow = ft_view_new(ft_surface_root_view(surface),
        &shadow_geometry);
    color.r = 100;
    color.g = 100;
    color.b = 100;
    color.a = 100;
    ft_view_set_color(window->shadow, &color);

    // Set resize.
    ft_rect_t resize_geometry = get_resize_geometry(window);
    window->resize = ft_view_new(window->shadow, &resize_geometry);

    // Set border.
    ft_rect_t border_geometry = get_border_geometry(window);
    window->border = ft_view_new(window->resize, &border_geometry);
    color.r = 0; // 100;
    color.g = 0; // 100;
    color.b = 0; // 100;
    color.a = 255;
    ft_view_set_color(window->border, &color);

    // Set title bar.
    ft_rect_t title_bar_geometry = get_title_bar_geometry(window);
    window->title_bar = ft_view_new(window->border, &title_bar_geometry);
    color.r = 180;
    color.g = 180;
    color.b = 180;
    color.a = 255;
    ft_view_set_color(window->title_bar, &color);

    ft_rect_t button_geometry;
    button_geometry.pos.x = 3;
    button_geometry.pos.y = 3;
    button_geometry.size.width = 24;
    button_geometry.size.height = 24;
    ft_view_t *close_button = ft_view_new(window->title_bar, &button_geometry);
    color.r = 255;
    color.g = 0;
    color.b = 0;
    ft_view_set_color(close_button, &color);
    ft_view_add_event_listener(close_button,
        FT_EVENT_TYPE_POINTER_PRESS,
        on_close_button_press);
    ft_view_add_event_listener(close_button,
        FT_EVENT_TYPE_POINTER_CLICK,
        on_close_button_click);

    button_geometry.pos.x = button_geometry.pos.x + 24 + 3;
    ft_view_t *minimize_button = ft_view_new(window->title_bar,
        &button_geometry);
    color.r = 255;
    color.g = 255;
    color.b = 0;
    ft_view_set_color(minimize_button, &color);

    button_geometry.pos.x = button_geometry.pos.x + 24 + 3;
    ft_view_t *maximize_button = ft_view_new(window->title_bar,
        &button_geometry);
    color.r = 0;
    color.g = 255;
    color.b = 0;
    ft_view_set_color(maximize_button, &color);

    // Set body.
    ft_rect_t body_geometry = get_body_geometry(window);
    window->body = ft_view_new(window->border, &body_geometry);
    color.r = 255;
    color.g = 255;
    color.b = 255;
    color.a = 255;
    ft_view_set_color(window->body, &color);
}

int main(int argc, char *argv[])
{
    ft_application_t *app = ft_application_new(argc, argv);

    struct window window;
    window.desktop_surface = ft_desktop_surface_new(
        FT_DESKTOP_SURFACE_ROLE_TOPLEVEL);

    init_window(&window);

    window_global = &window;

    // Desktop surface resize event handler.
    ft_desktop_surface_add_event_listener(
        window.desktop_surface,
        FT_EVENT_TYPE_RESIZE,
        on_desktop_surface_resize);

    // Desktop surface state change event handler.
    ft_desktop_surface_add_event_listener(
        window.desktop_surface,
        FT_EVENT_TYPE_STATE_CHANGE,
        on_desktop_surface_state_change);

    // Surface resize event handler.
    ft_surface_add_event_listener(
        ft_desktop_surface_surface(window.desktop_surface),
        FT_EVENT_TYPE_RESIZE,
        on_surface_resize);

    // Title bar press event handler.
    ft_view_add_event_listener(window.title_bar,
        FT_EVENT_TYPE_POINTER_PRESS,
        on_title_bar_press);

    ft_desktop_surface_show(window.desktop_surface);

    // Set the actual window size.
    ft_rect_t win_geometry = get_window_geometry(&window);
    ft_desktop_surface_set_wm_geometry(window.desktop_surface, &win_geometry);

    return ft_application_exec(app);
}

