#include <stdio.h>

#include <swingby/swingby.h>

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
    sb_view_radius_t radius = { 2.0f, 10.0f, 15.0f, 50.0f };
    sb_view_set_radius(view, &radius);

    sb_filter_t *blur = sb_filter_new(SB_FILTER_TYPE_BLUR);
    sb_filter_blur_set_radius(blur, 3.0f);
    sb_view_add_filter(view, blur);

    sb_desktop_surface_show(surface);

    return sb_application_exec(app);
}

