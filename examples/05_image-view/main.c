#include <stdio.h>

#include <swingby/swingby.h>

#include "image.h"

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
    sb_size_t init_size = { 300.0f, 300.0f };
    sb_surface_set_size(sb_desktop_surface_surface(surface), &init_size);

    sb_rect_t geometry = { { 10.0f, 10.0f }, { 256.0f, 256.0f } };
    sb_view_t *view = sb_view_new(
        sb_surface_root_view(sb_desktop_surface_surface(surface)), &geometry);
    image_view = view;
    sb_view_set_fill_type(view, SB_VIEW_FILL_TYPE_IMAGE);

    sb_image_t *image = sb_view_image(view);
    uint64_t len = sb_image_size(image)->width * sb_image_size(image)->height;
    fprintf(stderr, "Length: %ld\n", len);
    uint32_t *pixel = (uint32_t*)sb_image_data(image);
    for (uint32_t i = 0; i < len; ++i) {
        uint32_t r = image_data[i * 4 + 0] << 0;
        uint32_t g = image_data[i * 4 + 1] << 8;
        uint32_t b = image_data[i * 4 + 2] << 16;
        uint32_t a = image_data[i * 4 + 3] << 24;
        *pixel = r | g | b | a;
        ++pixel;
    }

    sb_surface_add_event_listener(sb_desktop_surface_surface(surface),
        SB_EVENT_TYPE_RESIZE,
        on_resize);

    sb_desktop_surface_show(surface);

    return sb_application_exec(app);
}

