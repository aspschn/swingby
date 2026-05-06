#include <stdlib.h>
#include <stdio.h>

#include <swingby/swingby.h>

#include "image.h"

sb_view_t *image_view = NULL;

/*
static uint8_t* load_image(int32_t *len)
{
    FILE *f = fopen("../../examples/05_image-view/miku@256x256.png", "rb");
    if (!f) {
        fprintf(stderr, "Failed to open image!\n");
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    int32_t size = ftell(f);
    *len = size;
    fseek(f, 0, SEEK_SET);

    uint8_t *data = malloc(size);
    fread(data, 1, size, f);

    fclose(f);

    return data;
}
*/

static void on_preferred_scale(sb_event_t *event, void *user_data)
{
    sb_surface_t *surface = event->target;

    fprintf(stderr, "on_preferred_scale: %d.\n", event->scale.scale);

    sb_surface_set_scale(surface, event->scale.scale);
}

static void on_resize(sb_event_t *event, void *user_data)
{
    fprintf(stderr, "on_resize\n");

    sb_rect_t geo;
    geo.pos.x = 10.0f;
    geo.pos.y = 10.0f;
    geo.size.width = sb_surface_size(event->target)->width - 10.0f - 10.0f;
    geo.size.height = sb_surface_size(event->target)->height - 10.0f - 10.0f;

    sb_view_set_geometry(image_view, &geo);
}

static void on_click(sb_event_t *event, void *user_data)
{
    return;
    sb_point_i_t pos;
    pos.x = (int32_t)event->pointer.position.x;
    pos.y = (int32_t)event->pointer.position.y;

    sb_size_i_t size;
    size.width = 80;
    size.height = 50;
    sb_image_t *rectangle = sb_image_new(&size, SB_IMAGE_FORMAT_RGBA32);
    sb_color_t color;
    color.r = 255;
    color.g = 0;
    color.b = 0;
    color.a = 128;
    sb_image_fill(rectangle, &color);

    enum sb_blend_mode blend = (event->pointer.button == SB_POINTER_BUTTON_LEFT)
        ? SB_BLEND_MODE_PREMULTIPLIED
        : SB_BLEND_MODE_NONE;
    sb_image_draw_image(sb_view_image(image_view), rectangle, &pos, blend);

    sb_image_free(rectangle);

    sb_surface_update(sb_view_surface(image_view));
}

int main(int argc, char *argv[])
{
    sb_application_t *app = sb_application_new(argc, argv);

    sb_desktop_surface_t *surface = sb_desktop_surface_new(
        SB_DESKTOP_SURFACE_ROLE_TOPLEVEL);
    sb_size_t init_size = { 300.0f, 300.0f };
    sb_surface_set_size(sb_desktop_surface_surface(surface), &init_size);

    sb_surface_add_event_listener(
        sb_desktop_surface_surface(surface),
        SB_EVENT_TYPE_PREFERRED_SCALE,
        on_preferred_scale,
        NULL);

    sb_rect_t geometry = { { 10.0f, 10.0f }, { 256.0f, 256.0f } };
    sb_view_t *view = sb_view_new(
        sb_surface_root_view(sb_desktop_surface_surface(surface)), &geometry);
    image_view = view;
    sb_view_set_render_type(view, SB_VIEW_RENDER_TYPE_IMAGE);
    sb_view_add_event_listener(view, SB_EVENT_TYPE_POINTER_CLICK, on_click, NULL);

    sb_size_i_t image_size = { 256, 256 };

    // sb_image_t *image = sb_image_new(&image_size, SB_IMAGE_FORMAT_RGBA32);
    // sb_image_t *image = sb_image_new_from_data(image_data, sizeof(image_data));
    // int32_t len;
    // uint8_t *data = load_image(&len);
    sb_pixmap_t *pixmap = sb_pixmap_new(image_data,
        &image_size, image_size.width * 4, SB_PIXEL_FORMAT_RGBA32);
    sb_image_t *image = sb_image_new_from_pixmap(pixmap);
    sb_view_set_image(view, image);

    /*
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
    */

    sb_desktop_surface_add_event_listener(surface,
        SB_EVENT_TYPE_RESIZE_REQUEST,
        on_resize,
        NULL);

    sb_desktop_surface_show(surface);

    return sb_application_exec(app);
}

