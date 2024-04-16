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

struct sb_cursor_t {
    sb_surface_t *surface;
    sb_cursor_shape shape;
    sb_point_t hot_spot;
};

sb_cursor_t* sb_cursor_new(sb_cursor_shape shape, const sb_point_t *hot_spot)
{
    sb_log_debug("sb_cursor_new()\n");
    sb_cursor_t *cursor = malloc(sizeof(sb_cursor_t));

    cursor->surface = sb_surface_new();
    cursor->shape = shape;
    cursor->hot_spot = *hot_spot;

    // Set size.
    sb_size_t size;
    size.width = 24.0f;
    size.height = 24.0f;
    sb_surface_set_size(cursor->surface, &size);

    // Set color for TEST.
    sb_color_t color;
    color.r = 255;
    color.g = 0;
    color.b = 0;
    color.a = 255;
    sb_view_set_color(sb_surface_root_view(cursor->surface), &color);

    sb_surface_attach(cursor->surface);
    sb_surface_commit(cursor->surface);

    return cursor;
}

sb_surface_t* sb_cursor_surface(sb_cursor_t *cursor)
{
    return cursor->surface;
}

#ifdef __cplusplus
}
#endif
