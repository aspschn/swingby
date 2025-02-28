#include <swingby/surface.h>

#include <stdlib.h>

#include <Windows.h>

#include <swingby/list.h>
#include <swingby/size.h>
#include <swingby/application.h>

#include "d3d-context/d3d-context.h"

#define SWINGBY_BACKEND_DEFAULT "raster"

struct sb_surface_t {
    HWND hwnd;
    sb_d3d_context_t *d3d_context;
    sb_size_t size;
    sb_list_t *event_listeners;
};



//!<=================
//!< Surface
//!<=================

sb_surface_t* sb_surface_new()
{
    sb_surface_t *surface = malloc(sizeof(sb_surface_t));

    // NULL initializations.
    surface->hwnd = NULL;
    surface->d3d_context = NULL;

    surface->size.width = 200.0f;
    surface->size.height = 200.0f;

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

    // Create D3D context.
    sb_d3d_global_context_t *d3d_global_context = sb_application_d3d_context(
        app);
    surface->d3d_context = sb_d3d_context_new();
    sb_d3d_context_init(surface->d3d_context, d3d_global_context);

    surface->event_listeners = sb_list_new();

    return surface;
}

const sb_size_t* sb_surface_size(sb_surface_t *surface)
{
    return &surface->size;
}

void sb_surface_set_size(sb_surface_t *surface, const sb_size_t *size)
{
    // TODO.
}

HWND sb_surface_hwnd(sb_surface_t *surface)
{
    return surface->hwnd;
}