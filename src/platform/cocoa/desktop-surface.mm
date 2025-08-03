#include <swingby/desktop-surface.h>

#import <Cocoa/Cocoa.h>

#ifdef __cplusplus
extern "C" {
#endif

struct sb_desktop_surface_t {
    sb_surface_t *surface;
};

sb_desktop_surface_t* sb_desktop_surface_new(sb_desktop_surface_role role)
{
    sb_desktop_surface_t* desktop_surface =
        (sb_desktop_surface_t*)malloc(sizeof(sb_desktop_surface_t));

    return desktop_surface;
}

void sb_desktop_surface_set_parent(sb_desktop_surface_t *desktop_surface,
                                   sb_desktop_surface_t *parent)
{}

void sb_desktop_surface_show(sb_desktop_surface_t *desktop_surface)
{}

void sb_desktop_surface_hide(sb_desktop_surface_t *desktop_surface)
{}

void sb_desktop_surface_popup_set_position(
    sb_desktop_surface_t *desktop_surface, const sb_point_t *position)
{}

void sb_desktop_surface_popup_grab_for_button(
    sb_desktop_surface_t *desktop_surface)
{}

void sb_desktop_surface_popup_grab_for_key(
    sb_desktop_surface_t *desktop_surface)
{}

void sb_desktop_surface_add_event_listener(
    sb_desktop_surface_t *desktop_surface,
    enum sb_event_type event_type,
    sb_event_listener_t listener,
    void *user_data)
{
}

void sb_desktop_surface_on_resize(sb_desktop_surface_t *desktop_surface,
                                  sb_event_t *event)
{
}

void sb_desktop_surface_on_state_change(sb_desktop_surface_t *desktop_surface,
                                        sb_event_t *event)
{}

sb_surface_t*
sb_desktop_surface_surface(sb_desktop_surface_t *desktop_surface)
{
    return desktop_surface->surface;
}

void sb_desktop_surface_free(sb_desktop_surface_t *desktop_surface)
{
    // TODO
}

void sb_desktop_surface_on_show(sb_desktop_surface_t *desktop_surface,
                                sb_event_t *event)
{}

void sb_desktop_surface_on_hide(sb_desktop_surface_t *desktop_surface,
                                sb_event_t *event)
{}

#ifdef __cplusplus
}
#endif
