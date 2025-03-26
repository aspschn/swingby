#include <stdio.h>

#include <swingby/swingby.h>

sb_view_t *item = NULL;

int main(int argc, char *argv[])
{
    sb_application_t *app = sb_application_new(argc, argv);

    sb_desktop_surface_t *surface = sb_desktop_surface_new(
        SB_DESKTOP_SURFACE_ROLE_TOPLEVEL);

    sb_rect_t geometry = { { 0.0f, 0.0f }, { 150.0f, 150.0f } };
    sb_view_t *view = sb_view_new(
        sb_surface_root_view(sb_desktop_surface_surface(surface)), &geometry);
    sb_color_t color = { 0, 255, 0, 255 };
    sb_view_set_color(view, &color);
    // Clip true.
    sb_view_set_clip(view, true);

    geometry.pos.x = 10.0f;
    geometry.pos.y = 10.0f;
    geometry.size.width = 50.0f;
    geometry.size.height = 50.0f;
    color.r = 255;
    color.g = 0;
    color.b = 0;
    item = sb_view_new(view, &geometry);
    sb_view_set_color(item, &color);

    sb_desktop_surface_t *popup = sb_desktop_surface_new(\
        SB_DESKTOP_SURFACE_ROLE_POPUP);
    sb_desktop_surface_set_parent(popup, surface);

    color.r = 255;
    color.g = 255;
    color.b = 0;
    color.a = 255;

    sb_surface_t *popup_surface = sb_desktop_surface_surface(popup);
    sb_surface_set_size(popup_surface, &geometry.size);

    sb_view_t *popup_root_view = sb_surface_root_view(popup_surface);
    sb_view_set_color(popup_root_view, &color);

    sb_desktop_surface_show(surface);

    sb_desktop_surface_show(popup);

    return sb_application_exec(app);
}

