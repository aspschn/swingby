#ifndef _SBGEARS_WINDOW_H
#define _SBGEARS_WINDOW_H

#include <swingby/swingby.h>

#define SBGEARS_WINDOW_BORDER_SIZE 6
#define SBGEARS_WINDOW_TITLE_BAR_SIZE 30

typedef struct sbgears_decoration sbgears_decoration;
typedef struct sbgears_window sbgears_window;

struct sbgears_decoration {
    sb_view_t *border;
    sb_view_t *title_bar;
    sb_view_t *close_button;
};

sbgears_decoration* decoration_new(const sbgears_window *window);


struct sbgears_window {
    sb_desktop_surface_t *sb_desktop_surface;
    sb_view_t *root_view;
    sbgears_decoration *decoration;
    sb_view_t *body;
};

sbgears_window* window_new();

void window_set_size(sbgears_window *window, const sb_size_i_t *size);

void window_free(sbgears_window *window);

#endif /* _SBGEARS_WINDOW_H */
