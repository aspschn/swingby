#include <stdio.h>

#include <swingby/swingby.h>

int main(int argc, char *argv[])
{
    sb_application_t *app = sb_application_new(argc, argv);

    sb_desktop_surface_t *surface = sb_desktop_surface_new(
        SB_DESKTOP_SURFACE_ROLE_TOPLEVEL);

    sb_size_t size;
    size.width = 200.0f;
    size.height = 200.0f;

    sb_surface_set_size(sb_desktop_surface_surface(surface), &size);

    sb_desktop_surface_t *child = sb_desktop_surface_new(
        SB_DESKTOP_SURFACE_ROLE_TOPLEVEL);

    size.width = 100.0f;
    size.height = 100.0f;

    sb_surface_set_size(sb_desktop_surface_surface(child), &size);

    // Child content.
    sb_rect_t rect;
    rect.size = size;
    rect.pos.x = 0;
    rect.pos.y = 0;
    sb_view_t *view = sb_view_new(
        sb_surface_root_view(sb_desktop_surface_surface(child)), &rect);
    sb_color_t color;
    color.r = 0;
    color.g = 255;
    color.b = 0;
    color.a = 255;
    sb_view_set_color(view, &color);

    sb_desktop_surface_set_parent(child, surface);

    sb_desktop_surface_show(surface);

    sb_desktop_surface_show(child);

    return sb_application_exec(app);
}

