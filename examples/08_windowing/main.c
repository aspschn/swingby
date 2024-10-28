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

int main(int argc, char *argv[])
{
    sb_application_t *app = sb_application_new(argc, argv);

    sb_size_t window_size;
    window_size.width = 200;
    window_size.height = 200;
    window = window_new(window_size);

    window_set_on_close_button_click(window, on_window_close_button_click);
    window_set_on_title_bar_press(window, on_window_title_bar_press);
    window_set_on_title_bar_release(window, on_window_title_bar_release);
    window_set_on_title_bar_pointer_move(window,
        on_window_title_bar_pointer_move);

    window_show(window);

    // Set the actual window size.
    // sb_rect_t win_geometry = get_window_geometry(&window);

    // sb_desktop_surface_set_wm_geometry(window.desktop_surface, &win_geometry);

    return sb_application_exec(app);
}
