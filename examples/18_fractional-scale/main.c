#include <stdio.h>

#include <swingby/swingby.h>

static sb_view_t *inner_global = NULL;

static void print_rect(const char *name, sb_rect_t rect)
{
    fprintf(stderr, "== %s ==\n", name);
    fprintf(stderr, " (%.2f, %.2f) %.2fx%.2f\n",
        rect.position.x, rect.position.y, rect.size.width, rect.size.height);
    fprintf(stderr, "========================\n");
}

static void print_rect_i(const char *name, sb_rect_i_t rect)
{
    fprintf(stderr, "== %s ==\n", name);
    fprintf(stderr, " (%d, %d) %dx%d\n",
        rect.position.x, rect.position.y, rect.size.width, rect.size.height);
    fprintf(stderr, "========================\n");
}

static void do_resize(sb_desktop_surface_t *toplevel, sb_size_t size)
{
    sb_surface_t *surface = sb_desktop_surface_surface(toplevel);
    float scale = sb_surface_scale(surface);
    const sb_view_t *root_view = sb_surface_root_view(surface);

    sb_view_set_geometry(inner_global, (sb_rect_t){
        .position = { .x = 1.0f, .y = 1.0f },
        .size = {
            .width = size.width,
            .height = size.height,
        },
    });

    sb_rect_t wm_geo = {
        .position = { .x = 1.0f, .y = 1.0f },
        .size = {
            .width = size.width,
            .height = size.height,
        },
    };

    sb_surface_set_size(surface, (sb_size_i_t){
        .width = size.width + 2,
        .height = size.height + 2,
    });
    sb_desktop_surface_set_wm_geometry(toplevel, &wm_geo);
    {
        print_rect("resize request",
            (sb_rect_t){
                .position = { .x = 0, .y = 0 },
                .size = size,
            });

        print_rect_i("surface logical",
            (sb_rect_i_t){
                .position = { .x = 0, .y = 0 },
                .size = sb_surface_size(surface),
            });

        print_rect("root view logical",
            sb_view_geometry(root_view));

        print_rect_i("root view physical",
            sb_view_physical_geometry(root_view));

        print_rect_i("surface buffer",
            (sb_rect_i_t){
                .position = { .x = 0, .y = 0 },
                .size = sb_surface_buffer_size(surface),
            });

        print_rect("wm geometry", wm_geo);

        print_rect("inner view", sb_view_geometry(inner_global));

        fprintf(stderr, "\n");
    }
}

static void on_resize_request(sb_event_t *event, void *user_data)
{
    do_resize(event->target, event->resize.size);
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

static void on_button_press(sb_event_t *event, void *user_data)
{
    sb_desktop_surface_t *toplevel = user_data;
    do_resize(toplevel, (sb_size_t){
        .width = sb_view_geometry(inner_global).size.width + 1.0f,
        .height = sb_view_geometry(inner_global).size.height,
    });
}

int main(int argc, char *argv[])
{
    sb_application_t *app = sb_application_new(argc, argv);

    sb_desktop_surface_t *toplevel = sb_desktop_surface_new(
        SB_DESKTOP_SURFACE_ROLE_TOPLEVEL);
    sb_surface_t *surface = sb_desktop_surface_surface(toplevel);

    sb_desktop_surface_add_event_listener(
        toplevel,
        SB_EVENT_TYPE_RESIZE_REQUEST,
        on_resize_request,
        NULL
    );

    sb_surface_add_event_listener(
        surface,
        SB_EVENT_TYPE_PREFERRED_SCALE,
        on_preferred_scale,
        NULL);

    sb_rect_t geometry = { { 0.0f, 0.0f }, { 200.0f, 200.0f } };
    sb_view_t *view = sb_view_new(
        sb_surface_root_view(surface), geometry);

    // Clip true.
    // sb_view_set_clip(view, true);

    // Inner rect.
    sb_view_t *inner = sb_view_new(view, (sb_rect_t){
        .position = { .x = 1.0f, .y = 1.0f },
        .size = { .width = 198.0f, .height = 198.0f },
    });
    sb_view_set_color(inner, (sb_color_t){
        .r = 0.5f, .g = 0.5f, .b = 0.5f, .a = 1.0f
    });
    sb_view_add_event_listener(inner, SB_EVENT_TYPE_POINTER_PRESS, on_button_press, toplevel);
    inner_global = inner;

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
        (void*)toplevel);

    sb_desktop_surface_show(toplevel);

    sb_desktop_surface_toplevel_set_title(toplevel,
        "18-fractional-scale — Swingby Example");

    return sb_application_exec(app);
}

