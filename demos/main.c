#include <stddef.h>
#include <stdio.h>

#include <GL/gl.h>

#include <swingby/swingby.h>

static void on_render(sb_event_t *event, void *user_data)
{
    printf("on_render");

    glViewport(0, 0, 100, 100);
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

int main(int argc, char *argv[])
{
    sb_application_t *app = sb_application_new(argc, argv);

    sb_desktop_surface_t *desktop_surface = sb_desktop_surface_new(
        SB_DESKTOP_SURFACE_ROLE_TOPLEVEL);

    sb_surface_t *surface = sb_desktop_surface_surface(desktop_surface);
    sb_view_t *root_view = sb_surface_root_view(surface);

    sb_rect_t geometry;
    geometry.pos.x = 0.0f;
    geometry.pos.y = 0.0f;
    geometry.size.width = 100.0f;
    geometry.size.height = 100.0f;

    sb_view_t *gl_view = sb_view_new(root_view, &geometry);
    sb_view_set_render_type(gl_view, SB_VIEW_RENDER_TYPE_GL);
    sb_view_add_event_listener(gl_view, SB_EVENT_TYPE_DIRECT_RENDER,
        on_render,
        NULL);

    sb_desktop_surface_show(desktop_surface);

    return sb_application_exec(app);
}
