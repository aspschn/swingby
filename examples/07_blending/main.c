#include <stdio.h>

#include <swingby/swingby.h>

int main(int argc, char *argv[])
{
    ft_application_t *app = ft_application_new(argc, argv);

    ft_desktop_surface_t *surface = ft_desktop_surface_new(
        FT_DESKTOP_SURFACE_ROLE_TOPLEVEL);

    ft_rect_t geometry = { { 0.0f, 0.0f }, { 100.0f, 100.0f } };
    ft_view_t *view = ft_view_new(
        ft_surface_root_view(ft_desktop_surface_surface(surface)), &geometry);
    ft_color_t color = { 255, 0, 0, 0x60 };
    ft_view_set_color(view, &color);
    fprintf(stderr, "First rect: #ff9f9f\n");

    geometry.pos.x = 10.0f;
    geometry.pos.y = 10.0f;
    geometry.size.width = 80.0f;
    geometry.size.height = 80.0f;
    ft_view_t *child_view = ft_view_new(view, &geometry);
    color.r = 0;
    color.g = 255;
    color.b = 0;
    color.a = 0x60;
    ft_view_set_color(child_view, &color);
    fprintf(stderr, "Second rect: #9fc363\n");

    geometry.pos.x = 10.0f;
    geometry.pos.y = 10.0f;
    geometry.size.width = 50.0f;
    geometry.size.height = 50.0f;
    ft_view_t *final_view = ft_view_new(child_view, &geometry);
    color.r = 0;
    color.g = 0;
    color.b = 255;
    color.a = 0x60;
    ft_view_set_color(final_view, &color);
    fprintf(stderr, "Final rect: #637a9e\n");

    ft_desktop_surface_show(surface);

    return ft_application_exec(app);
}

