#include <swingby/application.h>

#include <stdlib.h>

#include <Windows.h>

#include <swingby/list.h>
#include <swingby/event-dispatcher.h>

#include "d3d-context/d3d-context.h"

struct sb_application_t {
    WNDCLASS wc;
    sb_d3d_global_context_t *d3d_context;
    sb_list_t *desktop_surfaces;
    sb_event_dispatcher_t *event_dispatcher;
};

// Singleton object.
static sb_application_t *_sb_application_instance = NULL;

//!<=================
//!< Window Proc
//!<=================

static LRESULT CALLBACK WindowProc(HWND hwnd,
                                   UINT uMsg,
                                   WPARAM wParam,
                                   LPARAM lParam)
{
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

//!<====================
//!< Helper Functions
//!<====================


//!<===============
//!< Application
//!<===============

sb_application_t* sb_application_new(int argc, char *argv[])
{
    sb_application_t *app = malloc(sizeof(sb_application_t));

    // NULL initializations.
    app->d3d_context = NULL;

    // Init WNDCLASS and register it.
    HINSTANCE hInstance = GetModuleHandle(NULL);
    app->wc.lpfnWndProc = WindowProc;
    app->wc.hInstance = hInstance;
    app->wc.lpszClassName = "SwingbyWindowClass";
    RegisterClass(&app->wc);

    // Create D3D global context.
    app->d3d_context = sb_d3d_global_context_new();
    sb_d3d_global_context_init(app->d3d_context);

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

WNDCLASS* sb_application_wndclass(sb_application_t *application)
{
    return &application->wc;
}

int sb_application_exec(sb_application_t *application)
{
    MSG msg = {0,};
    while (GetMessage(&msg, NULL, 0, 0)) {
        DispatchMessage(&msg);
    }

    return 0;
}