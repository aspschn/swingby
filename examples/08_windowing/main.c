#include <stdio.h>

#include <swingby/swingby.h>

#include "window.h"

sb_view_t *image_view = NULL;

#define WINDOW_SHADOW_THICKNESS     40
#define WINDOW_RESIZE_THICKNESS     5
#define WINDOW_BORDER_THICKNESS     1
#define WINDOW_TITLE_BAR_HEIGHT     30
#define WINDOW_DEFAULT_WIDTH        200
#define WINDOW_DEFAULT_HEIGHT       200

static struct window *window = NULL;

static void on_window_close_button_click(sb_event_t *event)
{
    sb_desktop_surface_toplevel_close(window->desktop_surface);
}

static void on_window_minimize_button_click(sb_event_t *event)
{
    sb_desktop_surface_toplevel_set_minimized(window->desktop_surface);
}

static void on_window_maximize_restore_button_click(sb_event_t *event)
{
    if (window_maximized(window) == false) {
        sb_desktop_surface_toplevel_set_maximized(window->desktop_surface);
    } else {
        sb_desktop_surface_toplevel_unset_maximized(window->desktop_surface);
    }
}

static void on_window_title_bar_press(sb_event_t *event)
{
    window->decoration->title_bar->pressed = true;
    event->propagation = false;
}

static void on_window_title_bar_release(sb_event_t *event)
{
    window->decoration->title_bar->pressed = false;
    event->propagation = false;
}

static void on_window_title_bar_pointer_move(sb_event_t *event)
{
    if (window->decoration->title_bar->pressed) {
        sb_desktop_surface_toplevel_move(window->desktop_surface);
        window->decoration->title_bar->pressed = false;
    }
}

static void on_window_state_change(sb_event_t *event)
{
    enum sb_desktop_surface_toplevel_state state = event->state_change.state;
    bool value = event->state_change.value;
    sb_size_t size = event->state_change.size;

    sb_surface_t *surface = sb_desktop_surface_surface(window->desktop_surface);

    if (state == SB_DESKTOP_SURFACE_TOPLEVEL_STATE_MAXIMIZED &&
        value == true) {
        // Maximize.

        // Set surface size.
        sb_surface_set_size(surface, &size);
        // Set decoration.
        decoration_set_size(window->decoration, size);
        // Set body geometry.
        {
            sb_rect_t geometry;
            geometry.pos.x = window_body_offset_x(window);
            geometry.pos.y = window_body_offset_y(window);
            geometry.size = window_body_size_for(window, size);
            sb_view_set_geometry(window->body, &geometry);
        }
        // Set WM geometry.
        sb_rect_t geometry;
        geometry.size = event->state_change.size;
        geometry.pos.x = 0;
        geometry.pos.y = 0;
        sb_desktop_surface_set_wm_geometry(window->desktop_surface, &geometry);
    } else if (state == SB_DESKTOP_SURFACE_TOPLEVEL_STATE_MAXIMIZED &&
        value == false) {
        // Restore.

        // Set surface size.
        sb_size_t surface_size = window_surface_size_for(window, size);
        sb_surface_set_size(surface, &surface_size);
        // Set deoration.
        decoration_set_size(window->decoration, surface_size);
        // Set body geometry.
        {
            sb_rect_t geometry;
            geometry.pos.x = window_body_offset_x(window);
            geometry.pos.y = window_body_offset_y(window);
            geometry.size = window_body_size_for(window, size);
            sb_view_set_geometry(window->body, &geometry);
        }
        // Set WM geometry.
        sb_rect_t geometry;
        geometry.size = event->state_change.size;
        geometry.pos.x = window_decoration_border_offset(window);
        geometry.pos.y = window_decoration_border_offset(window);
        sb_desktop_surface_set_wm_geometry(window->desktop_surface, &geometry);
    }
}

int main(int argc, char *argv[])
{
    sb_application_t *app = sb_application_new(argc, argv);

    sb_size_t window_size;
    window_size.width = 200;
    window_size.height = 200;
    window = window_new(window_size);

    window_set_on_close_button_click(window, on_window_close_button_click);
    window_set_on_minimize_button_click(window,
        on_window_minimize_button_click);
    window_set_on_maximize_restore_button_click(window,
        on_window_maximize_restore_button_click);
    window_set_on_title_bar_press(window, on_window_title_bar_press);
    window_set_on_title_bar_release(window, on_window_title_bar_release);
    window_set_on_title_bar_pointer_move(window,
        on_window_title_bar_pointer_move);
    window_set_on_state_change(window, on_window_state_change);

    window_show(window);

    // Set the actual window size.
    // sb_rect_t win_geometry = get_window_geometry(&window);

    // sb_desktop_surface_set_wm_geometry(window.desktop_surface, &win_geometry);

    return sb_application_exec(app);
}
