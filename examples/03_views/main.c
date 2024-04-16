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
    fprintf(stderr, "view:          %p\n", view);

    geometry.pos.x = 10.0f;
    geometry.pos.y = 10.0f;
    geometry.size.width = 30.0f;
    geometry.size.height = 30.0f;
    sb_view_t *child_view = sb_view_new(view, &geometry);
    color.r = 0;
    color.g = 255;
    sb_view_set_color(child_view, &color);
    fprintf(stderr, "child_view:    %p\n", child_view);

    sb_desktop_surface_show(surface);

    return sb_application_exec(app);
}

