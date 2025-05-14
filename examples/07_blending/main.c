#include <stdio.h>

#include <swingby/swingby.h>

static void on_click(sb_event_t *event)
{
    sb_view_t *target = event->target;
    sb_point_t pos = sb_view_absolute_position(target);
    fprintf(stderr, "= Absolute pos: (%f, %f)\n", pos.x, pos.y);
}

int main(int argc, char *argv[])
{
    sb_application_t *app = sb_application_new(argc, argv);

    sb_desktop_surface_t *surface = sb_desktop_surface_new(
        SB_DESKTOP_SURFACE_ROLE_TOPLEVEL);

    sb_rect_t geometry = { { 0.0f, 0.0f }, { 100.0f, 100.0f } };
    sb_view_t *view = sb_view_new(
        sb_surface_root_view(sb_desktop_surface_surface(surface)), &geometry);
    sb_color_t color = { 255, 0, 0, 0x60 };
    sb_view_set_color(view, &color);
    fprintf(stderr, "First rect: #ff9f9f\n");

    geometry.pos.x = 10.0f;
    geometry.pos.y = 10.0f;
    geometry.size.width = 80.0f;
    geometry.size.height = 80.0f;
    sb_view_t *child_view = sb_view_new(view, &geometry);
    color.r = 0;
    color.g = 255;
    color.b = 0;
    color.a = 0x60;
    sb_view_set_color(child_view, &color);
    fprintf(stderr, "Second rect: #9fc363\n");

    geometry.pos.x = 10.0f;
    geometry.pos.y = 10.0f;
    geometry.size.width = 50.0f;
    geometry.size.height = 50.0f;
    sb_view_t *final_view = sb_view_new(child_view, &geometry);
    color.r = 0;
    color.g = 0;
    color.b = 255;
    color.a = 0x60;
    sb_view_set_color(final_view, &color);
    sb_view_add_event_listener(final_view, SB_EVENT_TYPE_POINTER_CLICK, on_click);
    fprintf(stderr, "Final rect: #637a9e\n");

    sb_desktop_surface_show(surface);

    return sb_application_exec(app);
}

