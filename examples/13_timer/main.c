#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include <swingby/swingby.h>

uint32_t timer_id;
sb_view_t *color_view;

static void on_preferred_scale(sb_event_t *event)
{
    sb_surface_t *surface = event->target;

    fprintf(stderr, "on_preferred_scale.\n");

    sb_surface_set_scale(surface, event->scale.scale);
}

static void on_timeout(sb_event_t *event)
{
    if (timer_id == event->timer.id) {
        srand(time(NULL));

        sb_color_t color;
        color.r = rand() % 255;
        color.g = rand() % 255;
        color.b = rand() % 255;
        color.a = 255;

        sb_view_set_color(color_view, &color);
    }
}

int main(int argc, char *argv[])
{
    sb_application_t *app = sb_application_new(argc, argv);

    sb_desktop_surface_t *surface = sb_desktop_surface_new(
        SB_DESKTOP_SURFACE_ROLE_TOPLEVEL);

    sb_rect_t geo;
    geo.pos.x = 10.0f;
    geo.pos.y = 10.0f;
    geo.size.width = 50.0f;
    geo.size.height = 50.0f;

    color_view = sb_view_new(
        sb_surface_root_view(sb_desktop_surface_surface(surface)),
        &geo
    );

    sb_surface_add_event_listener(
        sb_desktop_surface_surface(surface),
        SB_EVENT_TYPE_PREFERRED_SCALE,
        on_preferred_scale);

    timer_id = sb_application_add_timer(app,
        sb_desktop_surface_surface(surface),
        1000,
        true);

    sb_surface_add_event_listener(
        sb_desktop_surface_surface(surface),
        SB_EVENT_TYPE_TIMEOUT,
        on_timeout);

    sb_desktop_surface_show(surface);

    return sb_application_exec(app);
}

