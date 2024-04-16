#include <stdio.h>

#include <swingby/swingby.h>

ft_view_t *image_view = NULL;

void on_resize(ft_event_t *event)
{
    fprintf(stderr, "on_resize\n");

    ft_rect_t geo;
    geo.pos.x = 10.0f;
    geo.pos.y = 10.0f;
    geo.size.width = ft_surface_size(event->target)->width - 10.0f - 10.0f;
    geo.size.height = ft_surface_size(event->target)->height - 10.0f - 10.0f;

    ft_view_set_geometry(image_view, &geo);
}

int main(int argc, char *argv[])
{
    ft_application_t *app = ft_application_new(argc, argv);

    ft_desktop_surface_t *surface = ft_desktop_surface_new(
        FT_DESKTOP_SURFACE_ROLE_TOPLEVEL);

    ft_rect_t geometry = { { 10.0f, 10.0f }, { 101.0f, 101.0f } };
    ft_view_t *view = ft_view_new(
        ft_surface_root_view(ft_desktop_surface_surface(surface)), &geometry);
    image_view = view;
    ft_view_set_fill_type(view, FT_VIEW_FILL_TYPE_IMAGE);

    ft_image_t *image = ft_view_image(view);
    uint64_t len = ft_image_size(image)->width * ft_image_size(image)->height;
    uint32_t *pixel = (uint32_t*)ft_image_data(image);
    int red = 1;
    for (uint32_t i = 0; i < len; ++i) {
        if (red) {
            *pixel = 0xFF0000FF;
            red = 0;
        } else {
            *pixel = 0xFF00FF00;
            red = 1;
        }
        ++pixel;
    }

    ft_surface_add_event_listener(ft_desktop_surface_surface(surface),
        FT_EVENT_TYPE_RESIZE,
        on_resize);

    ft_desktop_surface_show(surface);

    return ft_application_exec(app);
}

