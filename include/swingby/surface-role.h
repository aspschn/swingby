#ifndef SWINGBY_SURFACE_ROLE_H
#define SWINGBY_SURFACE_ROLE_H

#include <swingby/event.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef struct sb_surface_t sb_surface_t;

typedef struct sb_surface_role_header_t sb_surface_role_header_t;

struct sb_surface_role_header_t {
    const char *name;
    sb_surface_t *surface;
    sb_event_listener_t event_filter;
};

const sb_surface_role_header_t* sb_surface_role_header(void *role);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* SWINGBY_SURFACE_ROLE_H */
