#include <stdio.h>

#include <swingby/swingby.h>

static void on_view_pointer_move(sb_event_t *event)
{
    event->propagation = false;

    const sb_list_t *filters = sb_view_filters((sb_view_t*)event->target);
    sb_filter_t *shadow_filter = NULL;
    for (int i = 0; i < sb_list_length(filters); ++i) {
        sb_filter_t *filter = sb_list_at(filters, i);
        if (sb_filter_type(filter) == SB_FILTER_TYPE_DROP_SHADOW) {
            shadow_filter = filter;
            break;
        }
    }
    sb_point_t center_pos;
    center_pos.x = sb_view_geometry(event->target)->size.width / 2.0f;
    center_pos.y = sb_view_geometry(event->target)->size.height / 2.0f;

    sb_point_t pos = event->pointer.position;
    pos.x = (center_pos.x - pos.x) / 2.0f;
    pos.y = (center_pos.y - pos.y) / 2.0f;

    sb_filter_drop_shadow_set_offset(shadow_filter, &pos);
    sb_surface_t *surface = sb_view_surface((sb_view_t*)event->target);
    sb_surface_update(surface);
}

int main(int argc, char *argv[])
{
    sb_application_t *app = sb_application_new(argc, argv);

    sb_desktop_surface_t *surface = sb_desktop_surface_new(
        SB_DESKTOP_SURFACE_ROLE_TOPLEVEL);

    sb_rect_t geometry = { { 20.0f, 20.0f }, { 80.0f, 80.0f } };
    sb_view_t *view = sb_view_new(
        sb_surface_root_view(sb_desktop_surface_surface(surface)), &geometry);
    sb_color_t color = { 255, 0, 0, 255 };
    sb_view_set_color(view, &color);
    sb_view_radius_t radius = { 2.0f, 10.0f, 15.0f, 50.0f };
    sb_view_set_radius(view, &radius);

    sb_filter_t *shadow = sb_filter_new(SB_FILTER_TYPE_DROP_SHADOW);
    sb_point_t offset = { 3.0f, 3.0f };
    sb_color_t shadow_color = { 0, 0, 0, 255 };
    sb_filter_drop_shadow_set_offset(shadow, &offset);
    sb_filter_drop_shadow_set_radius(shadow, 3.0f);
    sb_filter_drop_shadow_set_color(shadow, &shadow_color);
    sb_view_add_filter(view, shadow);

    sb_filter_t *blur = sb_filter_new(SB_FILTER_TYPE_BLUR);
    sb_filter_blur_set_radius(blur, 3.0f);
    sb_view_add_filter(view, blur);

    sb_view_add_event_listener(view,
                               SB_EVENT_TYPE_POINTER_MOVE,
                               on_view_pointer_move);

    sb_desktop_surface_show(surface);

    return sb_application_exec(app);
}
