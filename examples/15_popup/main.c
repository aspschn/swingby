#include <stdbool.h>
#include <stdio.h>

#include <swingby/swingby.h>

sb_view_t *item = NULL;

bool popup_on = false;
sb_desktop_surface_t *toplevel;
sb_desktop_surface_t *popup;

static void on_click(sb_event_t *event)
{
    if (popup_on == true) {
        sb_desktop_surface_hide(popup);
        sb_desktop_surface_free(popup);
        popup_on = false;
        return;
    }
    popup = sb_desktop_surface_new(
        SB_DESKTOP_SURFACE_ROLE_POPUP);
    sb_desktop_surface_set_parent(popup, toplevel);

    sb_color_t color;
    color.r = 255;
    color.g = 255;
    color.b = 0;
    color.a = 255;

    sb_rect_t geometry;
    geometry.pos.x = 0;
    geometry.pos.y = 0;
    geometry.size.width = 50;
    geometry.size.height = 50;

    sb_surface_t *popup_surface = sb_desktop_surface_surface(popup);
    sb_surface_set_size(popup_surface, &geometry.size);

    sb_view_t *popup_root_view = sb_surface_root_view(popup_surface);
    sb_view_set_color(popup_root_view, &color);
    sb_desktop_surface_show(popup);
    sb_desktop_surface_popup_grab_for_button(popup);

    popup_on = true;
}

int main(int argc, char *argv[])
{
    sb_application_t *app = sb_application_new(argc, argv);

    sb_desktop_surface_t *surface = sb_desktop_surface_new(
        SB_DESKTOP_SURFACE_ROLE_TOPLEVEL);
    toplevel = surface;

    sb_rect_t geometry = { { 0.0f, 0.0f }, { 150.0f, 150.0f } };
    sb_view_t *view = sb_view_new(
        sb_surface_root_view(sb_desktop_surface_surface(surface)), &geometry);
    sb_color_t color = { 0, 255, 0, 255 };
    sb_view_set_color(view, &color);
    // Clip true.
    sb_view_set_clip(view, true);

    geometry.pos.x = 10.0f;
    geometry.pos.y = 10.0f;
    geometry.size.width = 50.0f;
    geometry.size.height = 50.0f;
    color.r = 255;
    color.g = 0;
    color.b = 0;
    item = sb_view_new(view, &geometry);
    sb_view_set_color(item, &color);

    sb_view_add_event_listener(item, SB_EVENT_TYPE_POINTER_CLICK, on_click);

    sb_desktop_surface_show(surface);

    return sb_application_exec(app);
}

