#include <foundation/foundation.h>

int main(int argc, char *argv[])
{
    ft_application_t *app = ft_application_new(argc, argv);

    ft_desktop_surface_t *surface = ft_desktop_surface_new(
        FT_DESKTOP_SURFACE_ROLE_TOPLEVEL);

    ft_rect_t geometry = { { 10.0f, 10.0f }, { 50.0f, 50.0f } };
    ft_view_t *view = ft_view_new(
        ft_surface_root_view(ft_desktop_surface_surface(surface)), &geometry);
    ft_color_t color = { 255, 0, 0, 255 };
    ft_view_set_color(view, &color);

    geometry.pos.x = 5.0f;
    geometry.pos.y = 5.0f;
    geometry.size.width = 30.0f;
    geometry.size.height = 30.0f;
    ft_view_t *child_view = ft_view_new(view, &geometry);
    color.r = 0;
    color.g = 255;
    ft_view_set_color(child_view, &color);

    ft_desktop_surface_show(surface);

    return ft_application_exec(app);
}

