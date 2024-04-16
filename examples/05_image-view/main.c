#include <stdio.h>

#include <swingby/swingby.h>

sb_view_t *image_view = NULL;

void on_resize(sb_event_t *event)
{
    fprintf(stderr, "on_resize\n");

    sb_rect_t geo;
    geo.pos.x = 10.0f;
    geo.pos.y = 10.0f;
    geo.size.width = sb_surface_size(event->target)->width - 10.0f - 10.0f;
    geo.size.height = sb_surface_size(event->target)->height - 10.0f - 10.0f;

    sb_view_set_geometry(image_view, &geo);
}

int main(int argc, char *argv[])
{
    sb_application_t *app = sb_application_new(argc, argv);

    sb_desktop_surface_t *surface = sb_desktop_surface_new(
        SB_DESKTOP_SURFACE_ROLE_TOPLEVEL);

    sb_rect_t geometry = { { 10.0f, 10.0f }, { 101.0f, 101.0f } };
    sb_view_t *view = sb_view_new(
        sb_surface_root_view(sb_desktop_surface_surface(surface)), &geometry);
    image_view = view;
    sb_view_set_fill_type(view, SB_VIEW_FILL_TYPE_IMAGE);

    sb_image_t *image = sb_view_image(view);
    uint64_t len = sb_image_size(image)->width * sb_image_size(image)->height;
    uint32_t *pixel = (uint32_t*)sb_image_data(image);
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

    sb_surface_add_event_listener(sb_desktop_surface_surface(surface),
        SB_EVENT_TYPE_RESIZE,
        on_resize);

    sb_desktop_surface_show(surface);

    return sb_application_exec(app);
}

