#include <stdio.h>

#include <swingby/swingby.h>

const static sb_color_t bg = { 0.0f, 0.0f, 0.0f, 0.5f };
const static sb_color_t colors[] = {
    { 1.0f, 0.0f, 0.0f, 1.0f },
    { 1.0f, 0.6f, 0.0f, 1.0f },
    { 1.0f, 1.0f, 0.0f, 1.0f },
    { 0.0f, 1.0f, 0.0f, 1.0f },
    { 0.0f, 0.0f, 1.0f, 1.0f },
    { 1.0f, 0.0f, 1.0f, 1.0f },
};

static int surface_width = 200;
static int surface_height = 200;

static sb_point_t calc_distance()
{
    sb_point_t base = { .x = 20.0f, .y = 20.0f };

    sb_point_t ret;
    ret.x = (surface_width / 200.0f);
    ret.y = (surface_height / 200.0f);

    return ret;
}

static void on_resize_request(sb_event_t *event, void *user_data)
{
    sb_surface_t *surface = sb_desktop_surface_surface(event->target);

    sb_surface_set_size(surface, (sb_size_i_t){
        .width = event->resize.size.width,
        .height = event->resize.size.height
    });

    surface_width = event->resize.size.width;
    surface_height = event->resize.size.height;
}

static void on_preferred_scale(sb_event_t *event, void *user_data)
{
    sb_surface_t *surface = event->target;

    // fprintf(stderr, "on_preferred_scale.\n");

    sb_surface_set_scale(surface, event->scale.scale);
}

static void on_resizer_press(sb_event_t *event, void *user_data)
{
    sb_desktop_surface_t *toplevel = user_data;
    sb_desktop_surface_toplevel_resize(toplevel,
        SB_DESKTOP_SURFACE_TOPLEVEL_RESIZE_EDGE_LEFT);
}

static void on_paint(sb_event_t *event, void *user_data)
{
    sb_view_t *view = event->target;
    sb_canvas_t *canvas = sb_view_canvas(view);
    sb_paint_t *paint = sb_canvas_paint(canvas);
    printf("on_paint - view: %p, canvas: %p, paint: %p\n", view, canvas, paint);

    sb_rect_t rect;
    rect.position.x = 0.0f;
    rect.position.y = 0.0f;
    rect.size.width = 50.0f;
    rect.size.height = 50.0f;

    sb_point_t delta = calc_distance();

    for (int i = 0; i < sizeof(colors); ++i) {
        rect.position.x = (20.0f + (20.0f * i)) * delta.x;
        rect.position.y = (20.0f + (20.0f * i)) * delta.y;
        sb_paint_set_fill_color(paint, &colors[i]);

        sb_canvas_draw_rect(canvas, &rect, paint);
    }
}

int main(int argc, char *argv[])
{
    sb_application_t *app = sb_application_new(argc, argv);

    sb_desktop_surface_t *surface = sb_desktop_surface_new(
        SB_DESKTOP_SURFACE_ROLE_TOPLEVEL);

    sb_desktop_surface_add_event_listener(
        surface,
        SB_EVENT_TYPE_RESIZE_REQUEST,
        on_resize_request,
        NULL
    );

    sb_surface_add_event_listener(
        sb_desktop_surface_surface(surface),
        SB_EVENT_TYPE_PREFERRED_SCALE,
        on_preferred_scale,
        NULL);

    sb_rect_t geometry = { { 0.0f, 0.0f }, { 200.0f, 200.0f } };
    sb_view_t *view = sb_view_new(
        sb_surface_root_view(sb_desktop_surface_surface(surface)), geometry);
    sb_view_set_render_type(view, SB_VIEW_RENDER_TYPE_CANVAS);

    // Clip true.
    sb_view_set_clip(view, true);

    sb_view_add_event_listener(view, SB_EVENT_TYPE_PAINT, on_paint,
        NULL);

    // Resizer.
    sb_view_t *resizer = sb_view_new(view, (sb_rect_t){
        .position = { .x = 10.0f, .y = 30.0f },
        .size = { .width = 20.0f, .height = 30.0f },
    });
    sb_view_set_color(resizer, (sb_color_t){
        .r = 0.0f, .g = 0.0f, .b = 0.0f, .a = 1.0f
    });
    sb_view_add_event_listener(resizer, SB_EVENT_TYPE_POINTER_PRESS,
        on_resizer_press,
        (void*)surface);

    sb_desktop_surface_show(surface);

    sb_desktop_surface_toplevel_set_title(surface,
        "17-canvas — Swingby Example");

    return sb_application_exec(app);
}

