#include <swingby/surface.h>

#include <stdlib.h>

#import <Cocoa/Cocoa.h>

#include <swingby/list.h>
#include <swingby/application.h>
#include <swingby/view.h>

#include "../../helpers/shared.h"

#include "../../skia/context.h"

#ifdef __cplusplus
extern "C" {
#endif

struct sb_surface_t {
    NSWindow *ns_window;
    sb_skia_context_t *skia_context;
    sb_size_t size;
    sb_view_t *root_view;
    uint32_t scale;
    sb_list_t *event_listeners;
};

//!<=================
//!< Surface
//!<=================

sb_surface_t* sb_surface_new()
{
    sb_surface_t *surface = (sb_surface_t*)malloc(sizeof(sb_surface_t));

    // NULL initializations.
    surface->skia_context = NULL;
    surface->root_view = NULL;

    surface->size.width = 200.0f;
    surface->size.height = 200.0f;
    surface->scale = 1;

    // Create a Skia context.
    // TODO: "raster" | "metal"
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

    // Set root view's size.
    sb_rect_t new_geo;
    new_geo.pos.x = 0.0f;
    new_geo.pos.y = 0.0f;
    new_geo.size.width = size->width;
    new_geo.size.height = size->height;
    sb_view_set_geometry(surface->root_view, &new_geo);

    // Create and post resize event.
    sb_event_t *event = sb_event_new(SB_EVENT_TARGET_TYPE_SURFACE,
        surface, SB_EVENT_TYPE_RESIZE);
    // TODO: Set old_size.
    event->resize.size.width = size->width;
    event->resize.size.height = size->height;
    sb_application_post_event(sb_application_instance(), event);

    // TODO

    sb_surface_update(surface);
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
}

void sb_surface_update(sb_surface_t *surface)
{
    // _draw_frame(surface);
}

uint32_t sb_surface_scale(const sb_surface_t *surface)
{
    return surface->scale;
}

void sb_surface_set_scale(sb_surface_t *surface, uint32_t scale)
{
    surface->scale = scale;
}

sb_view_t* sb_surface_focused_view(const sb_surface_t *surface)
{
    return NULL;
}

void sb_surface_set_focused_view(sb_surface_t *surface, sb_view_t *view)
{
}

void sb_surface_add_event_listener(sb_surface_t *surface,
                                   enum sb_event_type event_type,
                                   sb_event_listener_t listener,
                                   void *user_data)
{
    sb_event_listener_tuple_t *tuple = sb_event_listener_tuple_new(
        event_type, listener, user_data);
    sb_list_push(surface->event_listeners, (void*)tuple);
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
    _event_listener_filter_for_each(surface->event_listeners,
        SB_EVENT_TYPE_RESIZE, event);
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


#ifdef __cplusplus
}
#endif