#include <swingby/application.h>

#include <stdlib.h>

#include <swingby/list.h>
#include <swingby/event-dispatcher.h>

#include "d3d-context/d3d-context.h"

struct sb_application_t {
    sb_d3d_global_context_t *d3d_context;
    sb_list_t *desktop_surfaces;
    sb_event_dispatcher_t *event_dispatcher;
};

// Singleton object.
static sb_application_t *_sb_application_instance = NULL;

//!<====================
//!< Helper Functions
//!<====================


//!<===============
//!< Application
//!<===============

sb_application_t* sb_application_new(int argc, char *argv[])
{
    sb_application_t *app = malloc(sizeof(sb_application_t));

    app->desktop_surfaces = sb_list_new();

    app->event_dispatcher = sb_event_dispatcher_new();

    _sb_application_instance = app;

    return app;
}

sb_application_t* sb_application_instance()
{
    return _sb_application_instance;
}
