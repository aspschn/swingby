#include <foundation/event-dispatcher.h>

#include <stdint.h>
#include <stdlib.h>

#include <foundation/log.h>
#include <foundation/bench.h>
#include <foundation/event.h>
#include <foundation/view.h>
#include <foundation/desktop-surface.h>

#ifdef __cplusplus
extern "C" {
#endif

//!<============
//!< Queue
//!<============

struct ft_queue_t {
    uint64_t capacity;
    uint64_t length;
    void **data;
};

ft_queue_t* ft_queue_new()
{
    ft_queue_t *queue = malloc(sizeof(ft_queue_t));

    queue->capacity = 16;
    queue->length = 0;

    queue->data = malloc(sizeof(void*));
    *queue->data = malloc(sizeof(void*) * queue->capacity);

    return queue;
}

void ft_queue_enqueue(ft_queue_t *queue, void *data)
{
    queue->data[queue->length] = data;

    queue->length += 1;
}

void* ft_queue_dequeue(ft_queue_t *queue)
{
    if (queue->length == 0) {
        return NULL;
    }

    void *data = queue->data[0];

    for (int i = 0; i < queue->length; ++i) {
        if (i == 0) {
            continue;
        }
        queue->data[i - 1] = queue->data[i];
    }

    queue->length -= 1;

    return data;
}

//!<===================
//!< Helper Functions
//!<===================

void _propagate_pointer_event(ft_view_t *view, ft_event_t *event)
{
    ft_view_t *parent = ft_view_parent(view);

    float x = event->pointer.position.x + ft_view_geometry(view)->pos.x;
    float y = event->pointer.position.y + ft_view_geometry(view)->pos.y;
    while (parent != NULL) {
        if (event->propagation == false) {
            break;
        }

        event->pointer.position.x = x;
        event->pointer.position.y = y;

        if (event->type == FT_EVENT_TYPE_POINTER_MOVE) {
            ft_view_on_pointer_move(parent, event);
        } else if (event->type == FT_EVENT_TYPE_POINTER_PRESS) {
            ft_view_on_pointer_press(parent, event);
        } else if (event->type == FT_EVENT_TYPE_POINTER_RELEASE) {
            ft_view_on_pointer_release(parent, event);
        } else if (event->type == FT_EVENT_TYPE_POINTER_CLICK) {
            ft_view_on_pointer_click(parent, event);
        }

        x = event->pointer.position.x + ft_view_geometry(parent)->pos.x;
        y = event->pointer.position.y + ft_view_geometry(parent)->pos.y;

        parent = ft_view_parent(parent);
    }
}

//!<===================
//!< Event Dispatcher
//!<===================

struct ft_event_dispatcher_t {
    ft_queue_t *queue;
};

ft_event_dispatcher_t* ft_event_dispatcher_new()
{
    ft_event_dispatcher_t *event_dispatcher = malloc(
        sizeof(ft_event_dispatcher_t));

    event_dispatcher->queue = ft_queue_new();

    return event_dispatcher;
}

void ft_event_dispatcher_post_event(ft_event_dispatcher_t *event_dispatcher,
                                    ft_event_t *event)
{
    ft_queue_enqueue(event_dispatcher->queue, (void*)event);
}

void
ft_event_dispatcher_process_events(ft_event_dispatcher_t *event_dispatcher)
{
    // ft_bench_t *bench = ft_bench_new("ft_event_dispatcher_process_events");

    while (event_dispatcher->queue->length != 0) {
        ft_event_t *event = (ft_event_t*)ft_queue_dequeue(
            event_dispatcher->queue);

        if (event->target_type == FT_EVENT_TARGET_TYPE_APPLICATION) {
            //
        } else if (event->target_type == FT_EVENT_TARGET_TYPE_DESKTOP_SURFACE) {
            switch (event->type) {
            case FT_EVENT_TYPE_RESIZE:
                ft_desktop_surface_on_resize(event->target, event);
                break;
            case FT_EVENT_TYPE_TOPLEVEL_STATE_CHANGE:
                ft_desktop_surface_on_toplevel_state_change(event->target,
                    event);
                break;
            default:
                break;
            }
        } else if (event->target_type == FT_EVENT_TARGET_TYPE_SURFACE) {
            switch (event->type) {
            case FT_EVENT_TYPE_POINTER_ENTER:
                ft_log_debug("Surface pointer enter!\n");
                break;
            case FT_EVENT_TYPE_POINTER_LEAVE:
                break;
            case FT_EVENT_TYPE_REQUEST_UPDATE:
                ft_surface_on_request_update(event->target);
                break;
            case FT_EVENT_TYPE_RESIZE:
                ft_surface_on_resize(event->target, event);
                break;
            default:
                break;
            }
        } else if (event->target_type == FT_EVENT_TARGET_TYPE_VIEW) {
            switch (event->type) {
            case FT_EVENT_TYPE_POINTER_ENTER:
                ft_log_debug("View pointer enter: (%f, %f) view: %p\n",
                    event->pointer.position.x, event->pointer.position.y,
                    event->target);
                ft_view_on_pointer_enter(event->target, event);
            case FT_EVENT_TYPE_POINTER_LEAVE:
                ft_view_on_pointer_leave(event->target, event);
                break;
            case FT_EVENT_TYPE_POINTER_MOVE:
                ft_view_on_pointer_move(event->target, event);
                _propagate_pointer_event(event->target, event);
                break;
            case FT_EVENT_TYPE_POINTER_PRESS:
                ft_view_on_pointer_press(event->target, event);
                _propagate_pointer_event(event->target, event);
                break;
            case FT_EVENT_TYPE_POINTER_RELEASE:
                ft_view_on_pointer_release(event->target, event);
                _propagate_pointer_event(event->target, event);
                break;
            case FT_EVENT_TYPE_POINTER_CLICK:
                ft_view_on_pointer_click(event->target, event);
                _propagate_pointer_event(event->target, event);
                break;
            case FT_EVENT_TYPE_POINTER_DOUBLE_CLICK:
                ft_view_on_pointer_double_click(event->target, event);
                _propagate_pointer_event(event->target, event);
            default:
                break;
            }
        }
    }

    // ft_bench_end(bench);
}

#ifdef __cplusplus
}
#endif
