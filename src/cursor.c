#include <swingby/cursor.h>

#include <stdlib.h>

#include <swingby/log.h>
#include <swingby/surface.h>
#include <swingby/size.h>
#include <swingby/color.h>
#include <swingby/view.h>

#ifdef __cplusplus
extern "C" {
#endif

struct ft_cursor_t {
    ft_surface_t *surface;
    ft_cursor_shape shape;
    ft_point_t hot_spot;
};

ft_cursor_t* ft_cursor_new(ft_cursor_shape shape, const ft_point_t *hot_spot)
{
    ft_log_debug("ft_cursor_new()\n");
    ft_cursor_t *cursor = malloc(sizeof(ft_cursor_t));

    cursor->surface = ft_surface_new();
    cursor->shape = shape;
    cursor->hot_spot = *hot_spot;

    // Set size.
    ft_size_t size;
    size.width = 24.0f;
    size.height = 24.0f;
    ft_surface_set_size(cursor->surface, &size);

    // Set color for TEST.
    ft_color_t color;
    color.r = 255;
    color.g = 0;
    color.b = 0;
    color.a = 255;
    ft_view_set_color(ft_surface_root_view(cursor->surface), &color);

    ft_surface_attach(cursor->surface);
    ft_surface_commit(cursor->surface);

    return cursor;
}

ft_surface_t* ft_cursor_surface(ft_cursor_t *cursor)
{
    return cursor->surface;
}

#ifdef __cplusplus
}
#endif
