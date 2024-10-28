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

int main(int argc, char *argv[])
{
    sb_application_t *app = sb_application_new(argc, argv);

    sb_size_t window_size;
    window_size.width = 200;
    window_size.height = 200;
    struct window *window = window_new(window_size);

    window_show(window);

    // Set the actual window size.
    // sb_rect_t win_geometry = get_window_geometry(&window);

    // sb_desktop_surface_set_wm_geometry(window.desktop_surface, &win_geometry);

    return sb_application_exec(app);
}
