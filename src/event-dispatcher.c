#include <foundation/event-dispatcher.h>

#include <stdint.h>
#include <stdlib.h>

#include <foundation/log.h>
#include <foundation/event.h>

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
    while (event_dispatcher->queue->length != 0) {
        ft_event_t *event = (ft_event_t*)ft_queue_dequeue(
            event_dispatcher->queue);

        if (event->target_type == FT_EVENT_TARGET_TYPE_APPLICATION) {
            //
        } else if (event->target_type == FT_EVENT_TARGET_TYPE_SURFACE) {
            switch (event->type) {
            case FT_EVENT_TYPE_POINTER_ENTER:
                ft_log_debug("Surface pointer enter!\n");
                break;
            case FT_EVENT_TYPE_POINTER_LEAVE:
                break;
            case FT_EVENT_TYPE_POINTER_MOVE:
                ft_log_debug("Surface pointer move event\n");
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
                break;
            default:
                break;
            }
        }
    }
}

#ifdef __cplusplus
}
#endif
