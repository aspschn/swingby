#include <swingby/surface.h>

#include <stdlib.h>

#include <swingby/list.h>

#include "d3d-context/d3d-context.h"

#define SWINGBY_BACKEND_DEFAULT "raster"

struct sb_surface_t {
    sb_d3d_context_t *d3d_context;
    sb_list_t *event_listeners;
};
