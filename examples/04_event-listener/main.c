#include <stdio.h>

#include <swingby/swingby.h>

static void on_click(sb_event_t *event, void *user_data)
{
    fprintf(stderr, "Click! (%.2f, %.2f)\n",
        event->pointer.position.x,
        event->pointer.position.y
    );
    // Prevent propagation.
    event->propagation = false;

    const sb_color_t *color = sb_view_color(event->target);
    sb_color_t new_color = *color;
    new_color.r += 100;
    new_color.b += 50;
    sb_view_set_color(event->target, &new_color);
}

static void on_double_click(sb_event_t *event, void *user_data)
{
    fprintf(stderr, "on_double_click\n");
    const sb_color_t *color = sb_view_color(event->target);
    sb_color_t new_color = *color;
    new_color.r += 100;
    new_color.g += 100;
    sb_view_set_color(event->target, &new_color);
}

int main(int argc, char *argv[])
{
    sb_application_t *app = sb_application_new(argc, argv);

    sb_desktop_surface_t *surface = sb_desktop_surface_new(
        SB_DESKTOP_SURFACE_ROLE_TOPLEVEL);

    sb_rect_t geometry = { { 10.0f, 10.0f }, { 50.0f, 50.0f } };
    sb_view_t *view = sb_view_new(
        sb_surface_root_view(sb_desktop_surface_surface(surface)), &geometry);
    sb_color_t color = { 255, 0, 0, 255 };
    sb_view_set_color(view, &color);

    geometry.pos.x = 10.0f;
    geometry.pos.y = 10.0f;
    geometry.size.width = 30.0f;
    geometry.size.height = 30.0f;
    sb_view_t *child_view = sb_view_new(view, &geometry);
    color.r = 0;
    color.g = 255;
    sb_view_set_color(child_view, &color);

    geometry.pos.x = 60.0f;
    geometry.pos.y = 10.0f;
    geometry.size.width = 30.0f;
    geometry.size.height = 30.0f;
    sb_view_t *double_click_view = sb_view_new(
        sb_surface_root_view(sb_desktop_surface_surface(surface)), &geometry);
    sb_view_set_color(double_click_view, &color);

    fprintf(stderr, "Click the smallest rectangle and see the change.\n");
    fprintf(stderr, "Double click the right rectangle and see the change.\n");

    sb_view_add_event_listener(child_view,
        SB_EVENT_TYPE_POINTER_CLICK, on_click, NULL);

    sb_view_add_event_listener(double_click_view,
        SB_EVENT_TYPE_POINTER_DOUBLE_CLICK, on_double_click, NULL);

    sb_desktop_surface_show(surface);

    return sb_application_exec(app);
}

