#include "window.h"

#include <stdlib.h>
#include <stdio.h>

static struct window *window_global;

static void on_window_resize_press(sb_event_t*);
static void on_window_resize_release(sb_event_t*);

struct window* window_new(sb_size_t size)
{
    struct window *window = malloc(sizeof(struct window));
    window_global = window;

    window->desktop_surface =
        sb_desktop_surface_new(SB_DESKTOP_SURFACE_ROLE_TOPLEVEL);

    sb_desktop_surface_add_event_listener(window->desktop_surface,
                                          SB_EVENT_TYPE_RESIZE,
                                          on_desktop_surface_resize);

    sb_surface_t *surface = sb_desktop_surface_surface(window->desktop_surface);
    sb_size_t initial_size;
    initial_size.width = size.width;
    initial_size.height = size.height;

    sb_surface_set_size(surface, &initial_size);

    // Set root view's color as transparent.
    sb_color_t trans;
    trans.r = 0;
    trans.g = 0;
    trans.b = 0;
    trans.a = 0;
    sb_view_set_color(sb_surface_root_view(surface), &trans);

    // Create decoration.
    window->decoration = decoration_new(window);
    title_bar_set_window(window->decoration->title_bar, window);
    sb_view_add_event_listener(window->decoration->resize.view,
                               SB_EVENT_TYPE_POINTER_PRESS,
                               on_window_resize_press);
    sb_view_add_event_listener(window->decoration->resize.view,
                               SB_EVENT_TYPE_POINTER_RELEASE,
                               on_window_resize_release);

    // Create body.
    sb_rect_t body_geometry;
    body_geometry.size.width = size.width;
    body_geometry.size.height = size.height;
    window->body = sb_view_new(sb_surface_root_view(surface), &body_geometry);

    // Set body geometry.
    body_geometry = window_body_geometry(window);
    sb_view_set_geometry(window->body, &body_geometry);

    // Update initial decoration geometry.
    decoration_set_size(window->decoration, window_whole_size(window));

    // Set window surface size.
    sb_size_t surface_size = window_whole_size(window);
    sb_surface_set_size(surface, &surface_size);

    return window;
}

void window_show(struct window *window)
{
    sb_desktop_surface_show(window->desktop_surface);

    // Window frame geometry hint.
    sb_rect_t frame_geometry = window_frame_geometry(window);
    sb_desktop_surface_set_wm_geometry(window->desktop_surface,
        &frame_geometry);
}

void window_maximize(struct window *window)
{
    //
}


void on_desktop_surface_resize(sb_event_t *event)
{
    fprintf(stderr, "Desktop surface resize: %fx%f\n",
            event->resize.size.width, event->resize.size.height);
    // Calculate the whole surface size.
    sb_size_t surface_size;
    surface_size.width =
        event->resize.size.width
        + (window_global->decoration->shadow.thickness * 2)
        - (window_global->decoration->border.thickness * 2);
    surface_size.height =
        event->resize.size.height
        + (window_global->decoration->shadow.thickness * 2)
        - (window_global->decoration->border.thickness * 2);

    // Set surface size.
    window_set_surface_size(window_global, surface_size);

    // Set decoration size.
    decoration_set_size(window_global->decoration, surface_size);

    // Set body geometry.
    const sb_rect_t *old_geometry = sb_view_geometry(window_global->body);
    sb_rect_t new_geometry;
    new_geometry.pos = old_geometry->pos;
    new_geometry.size = decoration_border_size(window_global->decoration);
    new_geometry.size.width -=
        (window_global->decoration->border.thickness * 2);
    new_geometry.size.height -=
        (window_global->decoration->border.thickness * 2)
        + window_global->decoration->title_bar->height;

    sb_view_set_geometry(window_global->body, &new_geometry);

    // Set window frame geometry hint.
    sb_rect_t frame_geometry = window_frame_geometry(window_global);
    sb_desktop_surface_set_wm_geometry(window_global->desktop_surface,
                                       &frame_geometry);
}

void window_set_on_close_button_click(struct window* window,
                                      void (*handler)(sb_event_t*))
{
    sb_view_add_event_listener(window->decoration->title_bar->close_button,
                               SB_EVENT_TYPE_POINTER_CLICK,
                               handler);
}

void window_set_on_title_bar_press(struct window *window,
                                   void (*handler)(sb_event_t*))
{
    sb_view_add_event_listener(window->decoration->title_bar->view,
                               SB_EVENT_TYPE_POINTER_PRESS,
                               handler);
}

void window_set_on_title_bar_release(struct window *window,
                                     void (*handler)(sb_event_t*))
{
    sb_view_add_event_listener(window->decoration->title_bar->view,
                               SB_EVENT_TYPE_POINTER_RELEASE,
                               handler);
}

void window_set_on_title_bar_pointer_move(struct window *window,
                                          void (*handler)(sb_event_t*))
{
    sb_view_add_event_listener(window->decoration->title_bar->view,
                               SB_EVENT_TYPE_POINTER_MOVE,
                               handler);
}

void window_set_on_state_change(struct window *window,
                                void (*handler)(sb_event_t*))
{
    sb_desktop_surface_add_event_listener(window->desktop_surface,
                                          SB_EVENT_TYPE_STATE_CHANGE,
                                          handler);
}


static void on_window_resize_press(sb_event_t *event)
{
    sb_desktop_surface_toplevel_resize(
        window_global->desktop_surface,
        SB_DESKTOP_SURFACE_TOPLEVEL_RESIZE_EDGE_BOTTOM_RIGHT
    );
}

static void on_window_resize_release(sb_event_t *event)
{
    // TODO.
}

//!<==============
//!< Properties
//!<==============

bool window_maximized(struct window *window)
{
    sb_desktop_surface_toplevel_state_flags states =
        sb_desktop_surface_toplevel_states(window->desktop_surface);

    return states & SB_DESKTOP_SURFACE_TOPLEVEL_STATE_MAXIMIZED;
}

//!<============
//!< Sizes
//!<============

sb_size_t window_body_size(struct window *window)
{
    const sb_rect_t *geometry = sb_view_geometry(window->body);

    return geometry->size;
}

sb_size_t window_frame_size(struct window *window)
{
    sb_size_t body_size = window_body_size(window);

    sb_size_t size;
    size.width = body_size.width + (window->decoration->border.thickness * 2);
    size.height = body_size.height + (window->decoration->border.thickness * 2);
    // Add title bar height.
    size.height += window->decoration->title_bar->height;

    return size;
}

sb_size_t window_whole_size(struct window *window)
{
    sb_size_t size = decoration_size(window->decoration, window);

    return size;
}

void window_set_surface_size(struct window *window, sb_size_t size)
{
    sb_surface_t *surface = sb_desktop_surface_surface(window->desktop_surface);

    sb_surface_set_size(surface, &size);
}

sb_size_t window_body_size_for(struct window *window, sb_size_t size)
{
    if (!window_maximized(window)) {
        sb_size_t body_size;
        body_size.width =
            size.width - (window->decoration->border.thickness * 2);
        body_size.height =
            size.height - (window->decoration->border.thickness * 2);
        body_size.height -= (window->decoration->title_bar->height);

        return body_size;
    } else {
        sb_size_t body_size;
        body_size.width = size.width;
        body_size.height =
            size.height - (window->decoration->title_bar->height);

        return body_size;
    }
}

sb_size_t window_surface_size_for(struct window *window, sb_size_t size)
{
    if (!window_maximized(window)) {
        sb_size_t surface_size;
        surface_size.width =
            size.width + (window->decoration->shadow.thickness * 2)
            - (window->decoration->border.thickness * 2);
        surface_size.height =
            size.height + (window->decoration->shadow.thickness * 2)
            - (window->decoration->border.thickness * 2);

        return surface_size;
    } else {
        return size;
    }
}

//!<============
//!< Geometry
//!<============

sb_rect_t window_body_geometry(struct window *window)
{
    sb_rect_t geometry;
    geometry.size = window_body_size(window);
    geometry.pos.x = window->decoration->shadow.thickness;
    geometry.pos.y = window->decoration->shadow.thickness;
    geometry.pos.y += window->decoration->title_bar->height;

    return geometry;
}

sb_rect_t window_frame_geometry(struct window *window)
{
    float shadow_thickness = window->decoration->shadow.thickness;
    float border_thickness = window->decoration->border.thickness;

    sb_rect_t geometry;
    geometry.size = window_frame_size(window);
    geometry.pos.x = shadow_thickness - border_thickness;
    geometry.pos.y = shadow_thickness - border_thickness;

    return geometry;
}

float window_decoration_border_offset(struct window *window)
{
    if (window_maximized(window)) {
        return 0.0f;
    }

    float offset = window->decoration->shadow.thickness;
    offset -= window->decoration->border.thickness;

    return offset;
}

float window_body_offset_x(struct window *window)
{
    if (window_maximized(window)) {
        return 0.0f;
    }

    float offset = window_decoration_border_offset(window);
    offset += window->decoration->border.thickness;

    return offset;
}

float window_body_offset_y(struct window *window)
{
    if (window_maximized(window)) {
        return window->decoration->title_bar->height;
    }

    float offset = window_decoration_border_offset(window);
    offset += window->decoration->border.thickness;
    offset += window->decoration->title_bar->height;

    return offset;
}

float window_decoration_title_bar_offset(struct window *window)
{
    if (window_maximized(window)) {
        return 0.0f;
    }

    float offset = window->decoration->shadow.thickness;

    return offset;
}
