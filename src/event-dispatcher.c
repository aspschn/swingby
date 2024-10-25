#include <swingby/event-dispatcher.h>

#include <stdint.h>
#include <stdlib.h>

#include <swingby/log.h>
#include <swingby/bench.h>
#include <swingby/event.h>
#include <swingby/view.h>
#include <swingby/desktop-surface.h>

#ifdef __cplusplus
extern "C" {
#endif

//!<============
//!< Queue
//!<============

struct sb_queue_t {
    uint64_t capacity;
    uint64_t length;
    void **data;
};

sb_queue_t* sb_queue_new()
{
    sb_queue_t *queue = malloc(sizeof(sb_queue_t));

    queue->capacity = 16;
    queue->length = 0;

    queue->data = malloc(sizeof(void**) * queue->capacity);

    return queue;
}

void sb_queue_enqueue(sb_queue_t *queue, void *data)
{
    queue->data[queue->length] = data;

    queue->length += 1;
}

void* sb_queue_dequeue(sb_queue_t *queue)
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

void _propagate_pointer_event(sb_view_t *view, sb_event_t *event)
{
    sb_view_t *parent = sb_view_parent(view);

    float x = event->pointer.position.x + sb_view_geometry(view)->pos.x;
    float y = event->pointer.position.y + sb_view_geometry(view)->pos.y;
    while (parent != NULL) {
        if (event->propagation == false) {
            break;
        }

        event->pointer.position.x = x;
        event->pointer.position.y = y;

        if (event->type == SB_EVENT_TYPE_POINTER_MOVE) {
            sb_view_on_pointer_move(parent, event);
        } else if (event->type == SB_EVENT_TYPE_POINTER_PRESS) {
            sb_view_on_pointer_press(parent, event);
        } else if (event->type == SB_EVENT_TYPE_POINTER_RELEASE) {
            sb_view_on_pointer_release(parent, event);
        } else if (event->type == SB_EVENT_TYPE_POINTER_CLICK) {
            sb_view_on_pointer_click(parent, event);
        }

        x = event->pointer.position.x + sb_view_geometry(parent)->pos.x;
        y = event->pointer.position.y + sb_view_geometry(parent)->pos.y;

        parent = sb_view_parent(parent);
    }
}

//!<===================
//!< Event Dispatcher
//!<===================

struct sb_event_dispatcher_t {
    sb_queue_t *queue;
};

sb_event_dispatcher_t* sb_event_dispatcher_new()
{
    sb_event_dispatcher_t *event_dispatcher = malloc(
        sizeof(sb_event_dispatcher_t));

    event_dispatcher->queue = sb_queue_new();

    return event_dispatcher;
}

void sb_event_dispatcher_post_event(sb_event_dispatcher_t *event_dispatcher,
                                    sb_event_t *event)
{
    sb_queue_enqueue(event_dispatcher->queue, (void*)event);
}

void
sb_event_dispatcher_process_events(sb_event_dispatcher_t *event_dispatcher)
{
    // sb_bench_t *bench = sb_bench_new("sb_event_dispatcher_process_events");

    while (event_dispatcher->queue->length != 0) {
        sb_event_t *event = (sb_event_t*)sb_queue_dequeue(
            event_dispatcher->queue);

        if (event->target_type == SB_EVENT_TARGET_TYPE_APPLICATION) {
            //
        } else if (event->target_type == SB_EVENT_TARGET_TYPE_DESKTOP_SURFACE) {
            switch (event->type) {
            case SB_EVENT_TYPE_RESIZE:
                sb_desktop_surface_on_resize(event->target, event);
                break;
            case SB_EVENT_TYPE_STATE_CHANGE:
                sb_desktop_surface_on_state_change(event->target,
                    event);
                break;
            default:
                break;
            }
        } else if (event->target_type == SB_EVENT_TARGET_TYPE_SURFACE) {
            switch (event->type) {
            case SB_EVENT_TYPE_POINTER_ENTER:
                sb_log_debug("Surface pointer enter!\n");
                break;
            case SB_EVENT_TYPE_POINTER_LEAVE:
                break;
            case SB_EVENT_TYPE_REQUEST_UPDATE:
                sb_surface_on_request_update(event->target);
                break;
            case SB_EVENT_TYPE_RESIZE:
                sb_surface_on_resize(event->target, event);
                break;
            default:
                break;
            }
        } else if (event->target_type == SB_EVENT_TARGET_TYPE_VIEW) {
            switch (event->type) {
            case SB_EVENT_TYPE_POINTER_ENTER:
                sb_log_debug("View pointer enter: (%f, %f) view: %p\n",
                    event->pointer.position.x, event->pointer.position.y,
                    event->target);
                sb_view_on_pointer_enter(event->target, event);
            case SB_EVENT_TYPE_POINTER_LEAVE:
                sb_view_on_pointer_leave(event->target, event);
                break;
            case SB_EVENT_TYPE_POINTER_MOVE:
                sb_view_on_pointer_move(event->target, event);
                _propagate_pointer_event(event->target, event);
                break;
            case SB_EVENT_TYPE_POINTER_PRESS:
                sb_view_on_pointer_press(event->target, event);
                _propagate_pointer_event(event->target, event);
                break;
            case SB_EVENT_TYPE_POINTER_RELEASE:
                sb_view_on_pointer_release(event->target, event);
                _propagate_pointer_event(event->target, event);
                break;
            case SB_EVENT_TYPE_POINTER_CLICK:
                sb_view_on_pointer_click(event->target, event);
                _propagate_pointer_event(event->target, event);
                break;
            case SB_EVENT_TYPE_POINTER_DOUBLE_CLICK:
                sb_view_on_pointer_double_click(event->target, event);
                _propagate_pointer_event(event->target, event);
            default:
                break;
            }
        }
    }

    // sb_bench_end(bench);
}

#ifdef __cplusplus
}
#endif
