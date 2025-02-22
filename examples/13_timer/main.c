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

    sb_application_add_timer(app,
        sb_desktop_surface_surface(surface),
        1000,
        true);

    sb_desktop_surface_show(surface);

    return sb_application_exec(app);
}

