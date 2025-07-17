#include <stdio.h>

#include <swingby/swingby.h>

sb_view_t *image_view = NULL;

#define WINDOW_SHADOW_THICKNESS     40
#define WINDOW_RESIZE_THICKNESS     5
#define WINDOW_BORDER_THICKNESS     1
#define WINDOW_TITLE_BAR_HEIGHT     30
#define WINDOW_DEFAULT_WIDTH        200
#define WINDOW_DEFAULT_HEIGHT       200

struct window {
    sb_desktop_surface_t *desktop_surface;
    sb_view_t *shadow;
    sb_view_t *resize;
    sb_view_t *border;
    sb_view_t *title_bar;
    sb_view_t *body;
};

struct window *window_global = NULL;

// `window_size` is from desktop surface resize event.
sb_size_t get_surface_size(sb_size_t *window_size)
{
    sb_size_t size;
    size.width = window_size->width + (WINDOW_SHADOW_THICKNESS * 2);
    size.height = window_size->height + (WINDOW_SHADOW_THICKNESS * 2);

    return size;
}

// Desktop surface wm_geometry.
sb_rect_t get_window_geometry(struct window *window)
{
    sb_surface_t *surface = sb_desktop_surface_surface(window->desktop_surface);
    const sb_size_t *surface_size = sb_surface_size(surface);

    sb_rect_t geometry;
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

sb_rect_t get_shadow_geometry(struct window *window)
{
    sb_surface_t *surface = sb_desktop_surface_surface(window->desktop_surface);
    const sb_size_t *surface_size = sb_surface_size(surface);

    sb_rect_t geometry;
    geometry.pos.x = 0;
    geometry.pos.y = 0;
    geometry.size.width = surface_size->width;
    geometry.size.height = surface_size->height;

    return geometry;
}

sb_rect_t get_resize_geometry(struct window *window)
{
    const sb_size_t shadow_size = sb_view_geometry(window->shadow)->size;

    sb_rect_t geometry;
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

sb_rect_t get_border_geometry(struct window *window)
{
    const sb_size_t resize_size = sb_view_geometry(window->resize)->size;

    sb_rect_t geometry;
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

sb_rect_t get_title_bar_geometry(struct window *window)
{
    const sb_size_t border_size = sb_view_geometry(window->border)->size;

    sb_rect_t geometry;
    geometry.pos.x = WINDOW_BORDER_THICKNESS;
    geometry.pos.y = WINDOW_BORDER_THICKNESS;
    geometry.size.width = border_size.width - (WINDOW_BORDER_THICKNESS * 2);
    geometry.size.height = WINDOW_TITLE_BAR_HEIGHT;

    return geometry;
}

sb_rect_t get_body_geometry(struct window *window)
{
    const sb_size_t border_size = sb_view_geometry(window->border)->size;

    sb_rect_t geometry;
    geometry.pos.x = WINDOW_SHADOW_THICKNESS;
    geometry.pos.y = WINDOW_SHADOW_THICKNESS + WINDOW_TITLE_BAR_HEIGHT;
    geometry.size.width = border_size.width - (WINDOW_BORDER_THICKNESS * 2);
    geometry.size.height = border_size.height - (WINDOW_BORDER_THICKNESS * 2)
        - WINDOW_TITLE_BAR_HEIGHT;

    return geometry;
}


static void on_desktop_surface_resize(sb_event_t *event, void *user_data)
{
    // Resize the surface.
    sb_surface_t *surface = sb_desktop_surface_surface(
        window_global->desktop_surface);
    sb_rect_t new_geo;
    new_geo.size = get_surface_size(&event->resize.size);

    sb_surface_set_size(surface, &new_geo.size);

    sb_rect_t wm_geo;
    wm_geo = get_window_geometry(window_global);
    sb_desktop_surface_set_wm_geometry(window_global->desktop_surface, &wm_geo);
}

static void on_desktop_surface_state_change(sb_event_t *event, void *user_data)
{
    fprintf(stderr, "State changed. %.2fx%.2f\n",
            event->state_change.size.width,
            event->state_change.size.height);
    sb_surface_t *surface = sb_desktop_surface_surface(
        window_global->desktop_surface);

    int state = event->state_change.state;
    if (state == SB_DESKTOP_SURFACE_TOPLEVEL_STATE_MAXIMIZED) {
        fprintf(stderr, " - Maximized: %d\n", event->state_change.value);
        if (event->state_change.value == true) {
            sb_size_t size;
            size.width = event->state_change.size.width;
            size.height = event->state_change.size.height;
            sb_surface_set_size(surface, &size);
        } else {
            sb_size_t size;
            size.width = event->state_change.size.width;
            size.height = event->state_change.size.height;
            sb_surface_set_size(surface, &size);
        }
    }
}

static void on_surface_resize(sb_event_t *event, void *user_data)
{
    fprintf(stderr, "on_resize: %dx%d\n", (int)event->resize.size.width, (int)event->resize.size.height);

    // Update shadow size.
    sb_rect_t shadow_geometry = get_shadow_geometry(window_global);
    sb_view_set_geometry(window_global->shadow, &shadow_geometry);

    // Update resize size.
    sb_rect_t resize_geometry = get_resize_geometry(window_global);
    sb_view_set_geometry(window_global->resize, &resize_geometry);

    // Update border size.
    sb_rect_t border_geometry = get_border_geometry(window_global);
    sb_view_set_geometry(window_global->border, &border_geometry);

    // Update title bar size.
    sb_rect_t title_bar_geometry = get_title_bar_geometry(window_global);
    sb_view_set_geometry(window_global->title_bar, &title_bar_geometry);

    // Update body size.
    sb_rect_t body_geometry = get_body_geometry(window_global);
    sb_view_set_geometry(window_global->body, &body_geometry);
}

static void on_close_button_press(sb_event_t *event, void *user_data)
{
    // Stop event propagation.
    event->propagation = false;
}

static void on_close_button_click(sb_event_t *event, void *user_data)
{
    fprintf(stderr, "Close button clicked.\n");
    sb_desktop_surface_toplevel_close(window_global->desktop_surface);
}

static void on_title_bar_press(sb_event_t *event, void *user_data)
{
    sb_desktop_surface_toplevel_move(window_global->desktop_surface);
}


static void init_window(struct window *window)
{
    sb_color_t color;
    sb_size_t size;
    sb_rect_t geometry;
    sb_surface_t *surface = sb_desktop_surface_surface(window->desktop_surface);

    // Set the total surface size.
    size.width = WINDOW_DEFAULT_WIDTH + (WINDOW_SHADOW_THICKNESS * 2);
    size.height = WINDOW_DEFAULT_HEIGHT + (WINDOW_SHADOW_THICKNESS * 2);
    sb_surface_set_size(surface, &size);

    // Transparent root view.
    color.r = 0;
    color.g = 0;
    color.b = 0;
    color.a = 0;
    sb_view_set_color(sb_surface_root_view(surface), &color);

    // Set shadow.
    sb_rect_t shadow_geometry = get_shadow_geometry(window);
    window->shadow = sb_view_new(sb_surface_root_view(surface),
        &shadow_geometry);
    color.r = 100;
    color.g = 100;
    color.b = 100;
    color.a = 100;
    sb_view_set_color(window->shadow, &color);

    // Set resize.
    sb_rect_t resize_geometry = get_resize_geometry(window);
    window->resize = sb_view_new(window->shadow, &resize_geometry);
    color.r = 0;
    color.g = 255;
    color.b = 0;
    color.a = 100;
    sb_view_set_color(window->resize, &color);

    // Set border.
    sb_rect_t border_geometry = get_border_geometry(window);
    window->border = sb_view_new(window->resize, &border_geometry);
    color.r = 0; // 100;
    color.g = 0; // 100;
    color.b = 0; // 100;
    color.a = 255;
    sb_view_set_color(window->border, &color);

    // Set title bar.
    sb_rect_t title_bar_geometry = get_title_bar_geometry(window);
    window->title_bar = sb_view_new(window->border, &title_bar_geometry);
    color.r = 180;
    color.g = 180;
    color.b = 180;
    color.a = 255;
    sb_view_set_color(window->title_bar, &color);

    sb_rect_t button_geometry;
    button_geometry.pos.x = 3;
    button_geometry.pos.y = 3;
    button_geometry.size.width = 24;
    button_geometry.size.height = 24;
    sb_view_t *close_button = sb_view_new(window->title_bar, &button_geometry);
    color.r = 255;
    color.g = 0;
    color.b = 0;
    sb_view_set_color(close_button, &color);
    sb_view_add_event_listener(close_button,
        SB_EVENT_TYPE_POINTER_PRESS,
        on_close_button_press,
        NULL);
    sb_view_add_event_listener(close_button,
        SB_EVENT_TYPE_POINTER_CLICK,
        on_close_button_click,
        NULL);

    button_geometry.pos.x = button_geometry.pos.x + 24 + 3;
    sb_view_t *minimize_button = sb_view_new(window->title_bar,
        &button_geometry);
    color.r = 255;
    color.g = 255;
    color.b = 0;
    sb_view_set_color(minimize_button, &color);

    button_geometry.pos.x = button_geometry.pos.x + 24 + 3;
    sb_view_t *maximize_button = sb_view_new(window->title_bar,
        &button_geometry);
    color.r = 0;
    color.g = 255;
    color.b = 0;
    sb_view_set_color(maximize_button, &color);

    // Set body.
    sb_rect_t body_geometry = get_body_geometry(window);
    window->body = sb_view_new(sb_surface_root_view(surface), &body_geometry);
    color.r = 255;
    color.g = 255;
    color.b = 255;
    color.a = 255;
    sb_view_set_color(window->body, &color);
}

int main(int argc, char *argv[])
{
    sb_application_t *app = sb_application_new(argc, argv);

    struct window window;
    window.desktop_surface = sb_desktop_surface_new(
        SB_DESKTOP_SURFACE_ROLE_TOPLEVEL);

    init_window(&window);

    window_global = &window;

    // Desktop surface resize event handler.
    sb_desktop_surface_add_event_listener(
        window.desktop_surface,
        SB_EVENT_TYPE_RESIZE,
        on_desktop_surface_resize,
        NULL);

    // Desktop surface state change event handler.
    sb_desktop_surface_add_event_listener(
        window.desktop_surface,
        SB_EVENT_TYPE_STATE_CHANGE,
        on_desktop_surface_state_change,
        NULL);

    // Surface resize event handler.
    sb_surface_add_event_listener(
        sb_desktop_surface_surface(window.desktop_surface),
        SB_EVENT_TYPE_RESIZE,
        on_surface_resize,
        NULL);

    // Title bar press event handler.
    sb_view_add_event_listener(window.title_bar,
        SB_EVENT_TYPE_POINTER_PRESS,
        on_title_bar_press,
        NULL);

    sb_desktop_surface_show(window.desktop_surface);

    // Set the actual window size.
    sb_rect_t win_geometry = get_window_geometry(&window);
    sb_desktop_surface_set_wm_geometry(window.desktop_surface, &win_geometry);

    return sb_application_exec(app);
}

