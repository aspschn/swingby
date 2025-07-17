#include "title-bar.h"

#include <stdlib.h>
// #include <stdio.h>

#include "window.h"

struct title_bar* title_bar_new(sb_view_t *parent)
{
    struct title_bar *title_bar = malloc(sizeof(struct title_bar));
    title_bar->height = TITLE_BAR_HEIGHT;
    title_bar->window = NULL;

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
                               on_button_press,
                               NULL);
    /*
    sb_view_add_event_listener(title_bar->close_button,
                               SB_EVENT_TYPE_POINTER_CLICK,
                               on_close_button_click);
    */
    sb_view_add_event_listener(title_bar->close_button,
                               SB_EVENT_TYPE_POINTER_ENTER,
                               on_close_button_pointer_enter,
                               NULL);
    sb_view_add_event_listener(title_bar->close_button,
                               SB_EVENT_TYPE_POINTER_LEAVE,
                               on_close_button_pointer_leave,
                               NULL);

    // Minimize button.
    geometry.pos.x += TITLE_BAR_BUTTON_SIZE + 3;
    title_bar->minimize_button = sb_view_new(title_bar->view, &geometry);
    button_color = minimize_button_color();
    sb_view_set_color(title_bar->minimize_button, &button_color);

    // Maximize/Restore button.
    geometry.pos.x += TITLE_BAR_BUTTON_SIZE + 3;
    title_bar->maximize_restore_button = sb_view_new(title_bar->view,
        &geometry);
    button_color.r = 0;
    button_color.g = 200;
    button_color.b = 0;
    button_color.a = 255;
    sb_view_set_color(title_bar->maximize_restore_button, &button_color);

    title_bar->pressed = false;

    return title_bar;
}

void title_bar_set_window(struct title_bar *title_bar, struct window *window)
{
    title_bar->window = window;
}


void on_button_press(sb_event_t *event, void *user_data)
{
    event->propagation = false;
}

void on_close_button_pointer_enter(sb_event_t *event, void *user_data)
{
    sb_view_t *close_button = (sb_view_t*)event->target;
    sb_color_t color = close_button_color_hover();
    sb_view_set_color(close_button, &color);
}

void on_close_button_pointer_leave(sb_event_t *event, void *user_data)
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

sb_color_t minimize_button_color()
{
    sb_color_t color;
    color.r = 255;
    color.g = 255;
    color.b = 0;
    color.a = 255;

    return color;
}


void title_bar_set_geometry(struct title_bar *title_bar, sb_rect_t geometry)
{
    // Height should not be changed.
    geometry.size.height = title_bar->height;
    sb_view_set_geometry(title_bar->view, &geometry);
}
