#include <swingby/surface.h>

#include <stdlib.h>

#include <Windows.h>

#include <swingby/list.h>
#include <swingby/size.h>
#include <swingby/application.h>
#include <swingby/view.h>
#include <swingby/log.h>

#include "d3d-context/d3d-context.h"

#include "../../skia/context.h"
#include "../../skia/draw.h"

#define SWINGBY_BACKEND_DEFAULT "raster"

struct sb_surface_t {
    HWND hwnd;
    sb_d3d_context_t *d3d_context;
    sb_skia_context_t *skia_context;
    sb_size_t size;
    sb_view_t *root_view;
    uint32_t scale;
    sb_list_t *event_listeners;
};

//!<=====================
//!< Helper Functions
//!<=====================

static void _draw_recursive(sb_surface_t *surface,
                            sb_view_t *view)
{
    enum sb_view_fill_type fill_type = sb_view_fill_type(view);

    if (fill_type == SB_VIEW_FILL_TYPE_SINGLE_COLOR) {
        const sb_view_radius_t *radius = NULL;
        const sb_list_t *filters = NULL;
        {
            // Set radius or NULL.
            if (!sb_view_radius_is_zero(sb_view_radius(view))) {
                radius = sb_view_radius(view);
            }
            // Set filters or NULL.
            if (sb_list_length((sb_list_t*)sb_view_filters(view))) {
                filters = sb_view_filters(view);
            }
        }
        sb_skia_draw_rect2(
            surface->skia_context,
            sb_view_geometry(view),
            surface->scale,
            sb_view_color(view),
            radius,
            filters,
            sb_view_clip(view)
        );
    } else if (fill_type == SB_VIEW_FILL_TYPE_IMAGE) {
        sb_skia_draw_image(surface->skia_context,
            sb_view_geometry(view), sb_view_image(view));
    }

    // Child views.
    sb_list_t *children = sb_view_children(view);
    for (int i = 0; i < sb_list_length(children); ++i) {
        if (sb_view_parent(view) != NULL) {
            sb_skia_save_pos(surface->skia_context, &sb_view_geometry(view)->pos);
        }

        sb_view_t *child = sb_list_at(children, i);
        _draw_recursive(surface, child);

        if (sb_view_parent(view) != NULL) {
            sb_skia_restore_pos(surface->skia_context);
        }
    }
}

void _draw_frame(sb_surface_t *surface)
{
    enum sb_skia_backend backend = sb_skia_context_backend(surface->skia_context);
    (void)backend;  // Currently do nothing with the backend type.

    // Skia context begin.
    sb_skia_context_set_buffer_size(surface->skia_context,
        surface->size.width * surface->scale,
        surface->size.height * surface->scale);
    sb_skia_context_begin(surface->skia_context,
        surface->size.width * surface->scale,
        surface->size.height * surface->scale);

    // Clear color.
    sb_color_t clear_color = { 0x00, 0x00, 0x00, 0x00 };
    sb_skia_clear(surface->skia_context, &clear_color);

    _draw_recursive(surface, surface->root_view);

    // Skia context end.
    sb_skia_context_end(surface->skia_context);

    // Copy buffer.
    void *data = sb_skia_context_buffer(surface->skia_context);
    sb_d3d_context_bitmap_copy_from_memory(
        surface->d3d_context,
        surface->size.width,
        surface->size.height,
        data
    );
}

//!<=================
//!< Surface
//!<=================

sb_surface_t* sb_surface_new()
{
    sb_surface_t *surface = malloc(sizeof(sb_surface_t));

    // NULL initializations.
    surface->hwnd = NULL;
    surface->d3d_context = NULL;
    surface->skia_context = NULL;
    surface->root_view = NULL;

    surface->size.width = 200.0f;
    surface->size.height = 200.0f;
    surface->scale = 1;

    sb_application_t *app = sb_application_instance();
    WNDCLASS *wc = sb_application_wndclass(app);

    // Create hwnd.
    surface->hwnd = CreateWindowEx(
        WS_EX_NOREDIRECTIONBITMAP,
        wc->lpszClassName,
        "",
        WS_POPUP,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        surface->size.width,
        surface->size.height,
        NULL,
        NULL,
        wc->hInstance,
        NULL
    );
    if (!IsWindow(surface->hwnd)) {
        sb_log_error("sb_surface_new - CreateWindowEx failed!\n");
    } else {
        sb_log_debug("sb_surface_new - CreateWindowEx done.\n");
    }

    // Create D3D context.
    sb_d3d_global_context_t *d3d_global_context = sb_application_d3d_context(
        app);
    surface->d3d_context = sb_d3d_context_new();
    sb_d3d_context_init(surface->d3d_context, d3d_global_context,
        surface->hwnd);
    sb_log_debug("sb_surface_new - D3D context init done.\n");

    // Create a Skia context.
    // Currently platform `win32` supports only raster backend.
    surface->skia_context = sb_skia_context_new(SB_SKIA_BACKEND_RASTER);

    // Root view.
    sb_rect_t geo;
    geo.pos.x = 0.0f;
    geo.pos.y = 0.0f;
    geo.size.width = surface->size.width;
    geo.size.height = surface->size.height;
    surface->root_view = sb_view_new(NULL, &geo);
    sb_view_set_surface(surface->root_view, surface);

    surface->event_listeners = sb_list_new();

    return surface;
}

const sb_size_t* sb_surface_size(sb_surface_t *surface)
{
    return &surface->size;
}

void sb_surface_set_size(sb_surface_t *surface, const sb_size_t *size)
{
    surface->size.width = size->width;
    surface->size.height = size->height;

    sb_d3d_context_swap_chain_resize_buffer(surface->d3d_context,
        size->width,
        size->height);
}

sb_view_t* sb_surface_root_view(sb_surface_t *surface)
{
    return surface->root_view;
}

void sb_surface_commit(sb_surface_t *surface)
{
    // TODO.
}

void sb_surface_attach(sb_surface_t *surface)
{
    sb_d3d_context_swap_chain_present(surface->d3d_context);
}

void sb_surface_update(sb_surface_t *surface)
{
    _draw_frame(surface);
}

HWND sb_surface_hwnd(sb_surface_t *surface)
{
    return surface->hwnd;
}

void sb_surface_on_pointer_enter(sb_surface_t *surface, sb_event_t *event)
{
}

void sb_surface_on_pointer_leave(sb_surface_t *surface, sb_event_t *event)
{
}

void sb_surface_on_request_update(sb_surface_t *surface)
{
}

void sb_surface_on_resize(sb_surface_t *surface, sb_event_t *event)
{
}

void sb_surface_on_keyboard_key_press(sb_surface_t *surface,
                                      sb_event_t *event)
{
}

void sb_surface_on_keyboard_key_release(sb_surface_t *surface,
                                        sb_event_t *event)
{
}

void sb_surface_on_preferred_scale(sb_surface_t *surface,
                                   sb_event_t *event)
{
}

void sb_surface_on_timeout(sb_surface_t *surface,
                           sb_event_t *event)
{
}
