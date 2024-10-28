#include "title-bar.h"

#include <stdlib.h>
// #include <stdio.h>

#include "window.h"

static struct title_bar *title_bar_global;

struct title_bar* title_bar_new(sb_view_t *parent)
{
    struct title_bar *title_bar = malloc(sizeof(struct title_bar));
    title_bar->height = TITLE_BAR_HEIGHT;
    title_bar->window = NULL;
    title_bar_global = title_bar;

    sb_rect_t geometry;
    geometry.pos.x = 0;
    geometry.pos.y = 0;
    geometry.size.width = 10.0;
    geometry.size.height = TITLE_BAR_HEIGHT;

    sb_view_t *view = sb_view_new(parent, &geometry);

    sb_color_t color;
    color.r = 180;
    color.g = 180;
    color.b = 180;
    color.a = 255;

    sb_view_set_color(view, &color);

    title_bar->view = view;

    // Close button.
    geometry.pos.x = 3;
    geometry.pos.y = 3;
    geometry.size.width = TITLE_BAR_BUTTON_SIZE;
    geometry.size.height = TITLE_BAR_BUTTON_SIZE;
    title_bar->close_button = sb_view_new(title_bar->view, &geometry);
    sb_color_t button_color = close_button_color();
    sb_view_set_color(title_bar->close_button, &button_color);

    sb_view_add_event_listener(title_bar->close_button,
                               SB_EVENT_TYPE_POINTER_PRESS,
                               on_button_press);
    sb_view_add_event_listener(title_bar->close_button,
                               SB_EVENT_TYPE_POINTER_CLICK,
                               on_close_button_click);
    sb_view_add_event_listener(title_bar->close_button,
                               SB_EVENT_TYPE_POINTER_ENTER,
                               on_close_button_pointer_enter);
    sb_view_add_event_listener(title_bar->close_button,
                               SB_EVENT_TYPE_POINTER_LEAVE,
                               on_close_button_pointer_leave);

    sb_view_add_event_listener(title_bar->view,
                               SB_EVENT_TYPE_POINTER_PRESS,
                               on_title_bar_press);
    sb_view_add_event_listener(title_bar->view,
                               SB_EVENT_TYPE_POINTER_RELEASE,
                               on_title_bar_release);
    sb_view_add_event_listener(title_bar->view,
                               SB_EVENT_TYPE_POINTER_MOVE,
                               on_title_bar_pointer_move);

    return title_bar;
}

void title_bar_set_window(struct title_bar *title_bar, struct window *window)
{
    title_bar->window = window;
}


void on_button_press(sb_event_t *event)
{
    event->propagation = false;
}

void on_close_button_click(sb_event_t *event)
{
    if (title_bar_global->window == NULL) {
        return;
    }

    sb_desktop_surface_toplevel_close(
        title_bar_global->window->desktop_surface);
}

void on_close_button_pointer_enter(sb_event_t *event)
{
    sb_view_t *close_button = (sb_view_t*)event->target;
    sb_color_t color = close_button_color_hover();
    sb_view_set_color(close_button, &color);
}

void on_close_button_pointer_leave(sb_event_t *event)
{
    sb_view_t *close_button = (sb_view_t*)event->target;
    sb_color_t color = close_button_color();
    sb_view_set_color(close_button, &color);
}


sb_color_t close_button_color()
{
    sb_color_t color;
    color.r = 255;
    color.g = 0;
    color.b = 0;
    color.a = 255;

    return color;
}

sb_color_t close_button_color_hover()
{
    sb_color_t color;
    color.r = 255;
    color.g = 100;
    color.b = 100;
    color.a = 255;

    return color;
}

void on_title_bar_press(sb_event_t *event)
{
    title_bar_global->pressed = true;
    event->propagation = false;
}

void on_title_bar_release(sb_event_t *event)
{
    title_bar_global->pressed = false;
    event->propagation = false;
}

void on_title_bar_pointer_move(sb_event_t *event)
{
    if (title_bar_global->pressed) {
        sb_desktop_surface_toplevel_move(title_bar_global->window->desktop_surface);
        title_bar_global->pressed = false;
    }
}


void title_bar_set_geometry(struct title_bar *title_bar, sb_rect_t geometry)
{
    // Height should not be changed.
    geometry.size.height = title_bar->height;
    sb_view_set_geometry(title_bar->view, &geometry);
}
