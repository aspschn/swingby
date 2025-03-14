#include <stdio.h>

#include <swingby/swingby.h>

sb_view_t *item = NULL;

static void on_preferred_scale(sb_event_t *event)
{
    sb_surface_t *surface = event->target;

    fprintf(stderr, "on_preferred_scale.\n");

    sb_surface_set_scale(surface, event->scale.scale);
}

static void on_scroll(sb_event_t *event)
{
    fprintf(stderr, "Scroll value: %.2f\n", event->scroll.value);
    const sb_rect_t *prev_geo = sb_view_geometry(item);
    sb_rect_t new_geo;
    new_geo.size = prev_geo->size;
    if (event->scroll.axis == SB_POINTER_SCROLL_AXIS_VERTICAL_SCROLL) {
        new_geo.pos.x = prev_geo->pos.x;
        new_geo.pos.y = prev_geo->pos.y + event->scroll.value;
    } else if (event->scroll.axis == SB_POINTER_SCROLL_AXIS_HORIZONTAL_SCROLL) {
        new_geo.pos.x = prev_geo->pos.x + event->scroll.value;
        new_geo.pos.y = prev_geo->pos.y;
    }

    sb_view_set_geometry(item, &new_geo);
}

int main(int argc, char *argv[])
{
    sb_application_t *app = sb_application_new(argc, argv);

    sb_desktop_surface_t *surface = sb_desktop_surface_new(
        SB_DESKTOP_SURFACE_ROLE_TOPLEVEL);

    sb_surface_add_event_listener(
        sb_desktop_surface_surface(surface),
        SB_EVENT_TYPE_PREFERRED_SCALE,
        on_preferred_scale);

    sb_rect_t geometry = { { 0.0f, 0.0f }, { 150.0f, 150.0f } };
    sb_view_t *view = sb_view_new(
        sb_surface_root_view(sb_desktop_surface_surface(surface)), &geometry);
    sb_color_t color = { 0, 255, 0, 255 };
    sb_view_set_color(view, &color);
    // Clip true.
    sb_view_set_clip(view, true);

    sb_view_add_event_listener(view, SB_EVENT_TYPE_POINTER_SCROLL, on_scroll);

    geometry.pos.x = 10.0f;
    geometry.pos.y = 10.0f;
    geometry.size.width = 50.0f;
    geometry.size.height = 50.0f;
    color.r = 255;
    color.g = 0;
    color.b = 0;
    item = sb_view_new(view, &geometry);
    sb_view_set_color(item, &color);

    sb_desktop_surface_show(surface);

    return sb_application_exec(app);
}

