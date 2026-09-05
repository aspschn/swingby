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

    // WM geometry.
    sb_rect_i_t wm_geo = {
        .position = { .x = 1, .y = 1 },
        .size = {
            .width = size.width,
            .height = size.height,
        },
    };
    // Surface size.
    sb_size_i_t surface_size = {
        .width = size.width + 2,
        .height = size.height + 2,
    };

    float remains = size.width * scale - (uint32_t)(size.width * scale);
    if (remains != 0.0f) {
        fprintf(stderr, "remains: %.2f\n", remains);
        if (remains == 0.25f) {
            wm_geo.size.width += 1;
            surface_size.width += 2;
        } else if (remains == 0.5f) {
            wm_geo.size.width += 2;
            surface_size.width += 2;
        } else if (remains == 0.75f) {
            wm_geo.size.width += 1;
            surface_size.width += 1;
        } else {
            return;
        }
    }

    sb_view_set_geometry(inner_global, (sb_rect_t){
        .position = { .x = 1.0f, .y = 1.0f },
        .size = {
            .width = size.width,
            .height = size.height,
        },
    });

    sb_desktop_surface_set_wm_geometry(toplevel, wm_geo);

    sb_surface_set_size(surface, surface_size);
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

        print_rect("root view scaled", (sb_rect_t){
            .position = {
                .x = sb_view_geometry(root_view).position.x * scale,
                .y = sb_view_geometry(root_view).position.y * scale,
            },
            .size = {
                .width = sb_view_geometry(root_view).size.width * scale,
                .height = sb_view_geometry(root_view).size.height * scale,
            },
        });

        print_rect_i("root view physical",
            sb_view_physical_geometry(root_view));

        print_rect_i("surface buffer",
            (sb_rect_i_t){
                .position = { .x = 0, .y = 0 },
                .size = sb_surface_buffer_size(surface),
            });

        print_rect_i("wm geometry", wm_geo);

        // print_rect("inner view", sb_view_geometry(inner_global));

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

static void on_wm_button_click(sb_event_t *event, void *user_data)
{
    sb_desktop_surface_t *toplevel = user_data;
    sb_surface_t *surface = sb_desktop_surface_surface(toplevel);

    sb_size_i_t size = sb_surface_size(surface);
    size.width += 1;
    sb_rect_i_t geo = {
        .position = { .x = 2, .y = 1 },
        .size = { .width = size.width, .height = size.height },
    };

    sb_surface_set_size(surface, size);
    sb_desktop_surface_set_wm_geometry(toplevel, geo);
    sb_surface_update(surface);
    geo.position.x = 1.0f;
    sb_desktop_surface_set_wm_geometry(toplevel, geo);
    sb_surface_update(surface);
}

int main(int argc, char *argv[])
{
    sb_application_t *app = sb_application_new(argc, argv);

    sb_desktop_surface_t *toplevel = sb_desktop_surface_new(
        SB_DESKTOP_SURFACE_ROLE_TOPLEVEL);
    sb_surface_t *surface = sb_desktop_surface_surface(toplevel);
    const sb_view_t *root_view = sb_surface_root_view(surface);

    // Set surface initial size.
    sb_surface_set_size(surface, (sb_size_i_t){
        .width = 200 + 2,
        .height = 200 + 2,
    });

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

    // Clip true.
    // sb_view_set_clip(view, true);

    // Inner rect.
    sb_view_t *inner = sb_view_new(root_view, (sb_rect_t){
        .position = { .x = 1.0f, .y = 1.0f },
        .size = { .width = 198.0f, .height = 198.0f },
    });
    sb_view_set_color(inner, (sb_color_t){
        .r = 0.0f, .g = 0.0f, .b = 0.0f, .a = 0.5f
    });
    inner_global = inner;

    // Resizer.
    sb_view_t *resizer = sb_view_new(root_view, (sb_rect_t){
        .position = { .x = 10.0f, .y = 30.0f },
        .size = { .width = 20.0f, .height = 30.0f },
    });
    sb_view_set_color(resizer, (sb_color_t){
        .r = 0.0f, .g = 0.0f, .b = 0.0f, .a = 1.0f
    });
    sb_view_add_event_listener(resizer, SB_EVENT_TYPE_POINTER_PRESS,
        on_resizer_press,
        (void*)toplevel);

    // Only WM geometry.
    sb_view_t *wm_button = sb_view_new(root_view, (sb_rect_t){
        .position = { .x = 40.0f, .y = 40.0f },
        .size = { .width = 2.0f, .height = 10.0f },
    });
    sb_view_set_color(wm_button, (sb_color_t){
        .r = 1.0f, .g = 0.0f, .b = 0.0f, .a = 1.0f
    });
    sb_view_add_event_listener(wm_button, SB_EVENT_TYPE_POINTER_CLICK,
        on_wm_button_click,
        (void*)toplevel);

    sb_desktop_surface_show(toplevel);

    // Minimum size.
    sb_size_t min = { .width = 200.0f, .height = 200.0f };
    sb_desktop_surface_toplevel_set_minimum_size(toplevel, &min);

    sb_desktop_surface_set_wm_geometry(toplevel, (sb_rect_i_t){
        .position = { .x = 1, .y = 1 },
        .size = { .width = 200, .height = 200 },
    });

    sb_desktop_surface_toplevel_set_title(toplevel,
        "18-fractional-scale — Swingby Example");

    return sb_application_exec(app);
}

