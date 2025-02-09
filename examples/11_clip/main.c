#include <stdio.h>

#include <swingby/swingby.h>

static void on_preferred_scale(sb_event_t *event)
{
    sb_surface_t *surface = event->target;

    fprintf(stderr, "on_preferred_scale.\n");

    sb_surface_set_scale(surface, event->scale.scale);
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

    sb_rect_t geometry = { { 10.0f, 10.0f }, { 80.0f, 80.0f } };
    sb_view_t *view = sb_view_new(
        sb_surface_root_view(sb_desktop_surface_surface(surface)), &geometry);
    sb_color_t color = { 255, 0, 0, 255 };
    sb_view_set_color(view, &color);
    sb_view_radius_t radius = { 2.0f, 10.0f, 15.0f, 50.0f };
    sb_view_set_radius(view, &radius);
    // First view is clipping.
    sb_view_set_clip(view, true);

    geometry.pos.x = -10.0f;
    geometry.pos.y = -10.0f;
    geometry.size.width = 50.0f;
    geometry.size.height = 50.0f;
    color.r = 0;
    color.g = 255;
    color.b = 0;
    sb_view_t *child = sb_view_new(view, &geometry);
    sb_view_set_color(child, &color);

    // Last view is not clipping.
    geometry.pos.x = 20.0f;
    geometry.pos.y = 20.0f;
    color.r = 0;
    color.g = 0;
    color.b = 255;
    sb_view_t *grandchild = sb_view_new(child, &geometry);
    sb_view_set_color(grandchild, &color);

    sb_desktop_surface_show(surface);

    return sb_application_exec(app);
}

