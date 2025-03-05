#include <swingby/application.h>

#include <stdlib.h>

#include <Windows.h>

#include <swingby/list.h>
#include <swingby/event.h>
#include <swingby/event-dispatcher.h>
#include <swingby/desktop-surface.h>
#include <swingby/surface.h>
#include <swingby/view.h>
#include <swingby/size.h>
#include <swingby/log.h>

#include "d3d-context/d3d-context.h"

#include "../../helpers/shared.h"

struct sb_application_t {
    WNDCLASS wc;
    sb_d3d_global_context_t *d3d_context;
    struct {
        sb_surface_t *surface;
        sb_view_t *view;
    } pointer;
    int nchittest_param;
    sb_list_t *desktop_surfaces;
    sb_event_dispatcher_t *event_dispatcher;
};

// Singleton object.
static sb_application_t *_sb_application_instance = NULL;


//!<====================
//!< Helper Functions
//!<====================

static sb_surface_t* _find_surface_by_hwnd(HWND hwnd)
{
    sb_application_t *app = sb_application_instance();

    sb_list_t *desktop_surfaces = app->desktop_surfaces;
    for (uint64_t i = 0; i < sb_list_length(desktop_surfaces); ++i) {
        sb_desktop_surface_t *desktop_surface = sb_list_at(desktop_surfaces, i);
        sb_surface_t *surface = sb_desktop_surface_surface(desktop_surface);
        if (hwnd == sb_surface_hwnd(surface)) {
            return surface;
        }
    }

    return NULL;
}

//!<=================
//!< Window Proc
//!<=================

static LRESULT CALLBACK WindowProc(HWND hwnd,
                                   UINT uMsg,
                                   WPARAM wParam,
                                   LPARAM lParam)
{
    sb_application_t *app = sb_application_instance();

    switch (uMsg) {
    case WM_NCHITTEST: {
        if (app->nchittest_param != 0) {
            sb_log_debug("WindowProc - WM_NCHITTEST - Value: %d\n",
                app->nchittest_param);
            return app->nchittest_param;
        } else {
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
        break;
    }
    case WM_SHOWWINDOW: {
        sb_log_debug("WindowProc - WM_SHOWWINDOW\n");
        sb_surface_t *surface = _find_surface_by_hwnd(hwnd);
        if (surface == NULL) {
            sb_log_warn("WindowProc - WM_SHOWWINDOW - Surface is NULL.\n");
            break;
        }

        // Initialize buffer size.
        const sb_size_t *size = sb_surface_size(surface);
        sb_surface_set_size(surface, size);

        break;
    }
    case WM_SIZE: {
        sb_log_debug("WindowProc - WM_SIZE\n");
        sb_surface_t *surface = _find_surface_by_hwnd(hwnd);
        if (surface == NULL) {
            break;
        }

        uint32_t width = LOWORD(lParam);
        uint32_t height = LOWORD(lParam);
        sb_size_t size;
        size.width = (float)width;
        size.height = (float)height;
        sb_surface_set_size(surface, &size);
        sb_surface_update(surface);

        break;
    }
    case WM_MOUSEMOVE: {
        sb_application_t *app = sb_application_instance();
        sb_surface_t *surface = _find_surface_by_hwnd(hwnd);
        sb_view_t *root_view = sb_surface_root_view(surface);

        // TODO: float point geometry.
        sb_point_t pos;
        pos.x = LOWORD(lParam);
        pos.y = HIWORD(lParam);

        sb_view_t *view = _find_most_child(root_view, &pos);
        // Pointer move event.
        {
            sb_event_t *event = sb_event_new(SB_EVENT_TARGET_TYPE_VIEW,
                view,
                SB_EVENT_TYPE_POINTER_MOVE);
            event->pointer.button = SB_POINTER_BUTTON_NONE;
            event->pointer.position.x = pos.x;
            event->pointer.position.y = pos.y;
            sb_application_post_event(app, event);
        }

        // Check difference.
        if (view != app->pointer.view) {
            sb_event_t *event = sb_event_new(SB_EVENT_TARGET_TYPE_VIEW,
                view, SB_EVENT_TYPE_POINTER_ENTER);
            event->pointer.button = SB_POINTER_BUTTON_NONE;
            event->pointer.position.x = pos.x;
            event->pointer.position.y = pos.y;
            sb_application_post_event(app, event);

            app->pointer.view = view;
        }

        break;
    }
    case WM_LBUTTONDOWN:
    {
        sb_point_t pos;
        pos.x = LOWORD(lParam);
        pos.y = HIWORD(lParam);
        sb_surface_t *surface = _find_surface_by_hwnd(hwnd);
        sb_view_t *view = _find_most_child(sb_surface_root_view(surface),
            &pos);

        sb_event_t *event = sb_event_new(SB_EVENT_TARGET_TYPE_VIEW,
            view, SB_EVENT_TYPE_POINTER_PRESS);
        event->pointer.button = SB_POINTER_BUTTON_LEFT;
        event->pointer.position.x = pos.x;
        event->pointer.position.y = pos.y;

        sb_application_post_event(app, event);

        ReleaseCapture();
        SendMessage(hwnd, WM_NCLBUTTONDOWN, HTCAPTION, lParam);

        break;
    }
    default:
        break;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}


//!<===============
//!< Application
//!<===============

sb_application_t* sb_application_new(int argc, char *argv[])
{
    sb_application_t *app = malloc(sizeof(sb_application_t));

    // NULL initializations.
    app->d3d_context = NULL;

    app->nchittest_param = 0;

    // Init WNDCLASS and register it.
    HINSTANCE hInstance = GetModuleHandle(NULL);
    memset(&app->wc, 0, sizeof(WNDCLASS));
    app->wc.lpfnWndProc = WindowProc;
    app->wc.hInstance = hInstance;
    app->wc.lpszClassName = "SwingbyWindowClass";
    RegisterClass(&app->wc);

    // Create D3D global context.
    app->d3d_context = sb_d3d_global_context_new();
    sb_d3d_global_context_init(app->d3d_context);

    // NULL init pointer event struct.
    app->pointer.surface = NULL;
    app->pointer.view = NULL;

    app->desktop_surfaces = sb_list_new();

    app->event_dispatcher = sb_event_dispatcher_new();

    _sb_application_instance = app;

    return app;
}

sb_application_t* sb_application_instance()
{
    return _sb_application_instance;
}

void sb_application_register_desktop_surface(sb_application_t *application,
    sb_desktop_surface_t *desktop_surface)
{
    sb_list_push(application->desktop_surfaces, (void*)desktop_surface);
}

sb_d3d_global_context_t* sb_application_d3d_context(
    sb_application_t *application)
{
    return application->d3d_context;
}

void sb_application_post_event(sb_application_t *application,
                               sb_event_t *event)
{
    sb_event_dispatcher_post_event(application->event_dispatcher, event);
}

WNDCLASS* sb_application_wndclass(sb_application_t *application)
{
    return &application->wc;
}

void sb_application_set_nchittest_return(sb_application_t *application,
                                         int value)
{
    application->nchittest_param = value;
}

int sb_application_exec(sb_application_t *application)
{
    int err;

    MSG msg = {0,};
    err = GetMessage(&msg, NULL, 0, 0);
    while (err > 0) {
        DispatchMessage(&msg);
        sb_event_dispatcher_process_events(application->event_dispatcher);

        err = GetMessage(&msg, NULL, 0, 0);
    }

    if (err == -1) {
        return 1;
    }

    return 0;
}
