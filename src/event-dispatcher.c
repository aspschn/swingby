#include <swingby/event-dispatcher.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include <swingby/common.h>
#include <swingby/log.h>
#include <swingby/bench.h>
#include <swingby/event.h>
#include <swingby/list.h>
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

        // Maybe next line required!
        // event->target = parent;
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
        } else if (event->type == SB_EVENT_TYPE_POINTER_SCROLL) {
            sb_view_on_pointer_scroll(parent, event);
        }

        x = event->pointer.position.x + sb_view_geometry(parent)->pos.x;
        y = event->pointer.position.y + sb_view_geometry(parent)->pos.y;

        parent = sb_view_parent(parent);
    }
}

//!<===================
//!< Event Dispatcher
//!<===================

typedef struct sb_repeat_event_t {
    sb_event_t *event;
    /// \brief Free it next repeat time.
    sb_event_t *sent_event;
    uint64_t time;
    bool repeating;
} sb_repeat_event_t;

sb_repeat_event_t* sb_repeat_event_new(sb_event_t *event)
{
    sb_repeat_event_t *repeat_event = malloc(sizeof(sb_repeat_event_t));

    repeat_event->event = event;
    repeat_event->sent_event = NULL;
    repeat_event->time = 0;
    repeat_event->repeating = false;

    return repeat_event;
}

void sb_repeat_event_free(sb_repeat_event_t *repeat_event)
{
    repeat_event->event = NULL;

    if (repeat_event->event != NULL) {
        sb_event_free(repeat_event->event);
        repeat_event->event = NULL;
    }
    if (repeat_event->sent_event != NULL) {
        sb_event_free(repeat_event->sent_event);
        repeat_event->sent_event = NULL;
    }
    free(repeat_event);
}

static void _print_repeat_event_list(sb_list_t *list)
{
    for (int i = 0; i < sb_list_length(list); ++i) {
        sb_repeat_event_t *repeat_event = sb_list_at(list, i);
        sb_log_debug(" i: %d - keycode: %d, event: %p\n", i,
                     repeat_event->event->keyboard.keycode,
                     repeat_event->event);
    }
}

struct sb_event_dispatcher_t {
    sb_queue_t *queue;
    struct {
        uint32_t delay;
        uint32_t rate;
        /// \brief List for sb_repeat_event_t.
        sb_list_t *events;
    } keyboard_key_repeat;
    struct {
        /// \brief List for sb_event_t.
        sb_list_t *events;
    } timer;
};

sb_event_dispatcher_t* sb_event_dispatcher_new()
{
    sb_event_dispatcher_t *event_dispatcher = malloc(
        sizeof(sb_event_dispatcher_t));

    event_dispatcher->queue = sb_queue_new();

    // Initialize keyboard key repeat info.
    event_dispatcher->keyboard_key_repeat.delay = 100000;
    event_dispatcher->keyboard_key_repeat.rate = 0;
    event_dispatcher->keyboard_key_repeat.events = sb_list_new();

    event_dispatcher->timer.events = sb_list_new();

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
            case SB_EVENT_TYPE_KEYBOARD_KEY_PRESS:
                sb_surface_on_keyboard_key_press(event->target, event);
                // sb_event_free(event);
                break;
            case SB_EVENT_TYPE_KEYBOARD_KEY_RELEASE:
                sb_surface_on_keyboard_key_release(event->target, event);
                // sb_event_free(event);
                break;
            case SB_EVENT_TYPE_PREFERRED_SCALE:
                sb_surface_on_preferred_scale(event->target, event);
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
                break;
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
                break;
            case SB_EVENT_TYPE_POINTER_SCROLL:
                sb_view_on_pointer_scroll(event->target, event);
                _propagate_pointer_event(event->target, event);
                sb_event_free(event);
                break;
            default:
                break;
            }
        }
    }

    // Process keyboard key repeat.
    sb_list_t *event_list = event_dispatcher->keyboard_key_repeat.events;
    uint64_t delay = event_dispatcher->keyboard_key_repeat.delay;
    uint64_t rate = event_dispatcher->keyboard_key_repeat.rate;
    // For each key events.
    for (int i = 0; i < sb_list_length(event_list); ++i) {
        sb_repeat_event_t *repeat_event = sb_list_at(event_list, i);
        sb_event_t *evt = repeat_event->event;
        if (evt != NULL) {
            uint64_t time = repeat_event->time;

            if (!repeat_event->repeating) {
                // Not repeating, wait until the time hit.
                uint64_t now = sb_time_now_milliseconds();
                if (now - time >= delay) {
                    repeat_event->repeating = true;
                    repeat_event->time = now;
                }
            } else {
                // If repeating, make an event for the repeat info.
                uint64_t now = sb_time_now_milliseconds();
                if (now - time >= rate) {
                    repeat_event->time = now;
                    sb_log_debug("Key repeat: %d\n", evt->keyboard.keycode);
                    // New event.
                    sb_event_t *new_event = sb_event_new(evt->target_type,
                        evt->target,
                        evt->type);
                    switch (new_event->target_type) {
                    case SB_EVENT_TARGET_TYPE_SURFACE:
                        sb_surface_on_keyboard_key_press(new_event->target,
                            new_event);
                        break;
                    case SB_EVENT_TARGET_TYPE_VIEW:
                        // sb_view_on_keyboard_key_press(new_event->target,
                        //     new_event);
                        break;
                    default:
                        break;
                    }
                    // Free the previous sent event.
                    {
                        if (repeat_event->sent_event != NULL) {
                            sb_event_free(repeat_event->sent_event);
                            repeat_event->sent_event = NULL;
                        }
                    }
                    // Set sent event as the new event.
                    {
                        repeat_event->sent_event = new_event;
                    }
                }
            }
        }
    }

    // Process timer events.
    sb_list_t *timer_event_list = event_dispatcher->timer.events;
    for (uint64_t i = 0; i < sb_list_length(timer_event_list); ++i) {
        sb_event_t *event = sb_list_at(timer_event_list, i);
        uint64_t now = sb_time_now_milliseconds();
        if (now - event->timer.time >= event->timer.interval) {
            sb_log_debug("Timer triggered!\n");
            // Call on_timeout method of the surface.
            sb_surface_on_timeout(event->target, event);
            // If no repeat, remove event.
            if (event->timer.repeat == false) {
                sb_event_dispatcher_timer_remove_event(event_dispatcher,
                    event->timer.id);
            }
            event->timer.time = now;
        }
    }

    // sb_bench_end(bench);
}

void sb_event_dispatcher_keyboard_key_repeat_set_delay(
    sb_event_dispatcher_t *event_dispatcher, uint32_t delay)
{
    event_dispatcher->keyboard_key_repeat.delay = delay;
}

void sb_event_dispatcher_keyboard_key_repeat_set_rate(
    sb_event_dispatcher_t *event_dispatcher, uint32_t rate)
{
    event_dispatcher->keyboard_key_repeat.rate = rate;
}

bool sb_event_dispatcher_keyboard_key_repeat_has_event(
    sb_event_dispatcher_t *event_dispatcher)
{
    return sb_list_length(event_dispatcher->keyboard_key_repeat.events) > 0;
}

void sb_event_dispatcher_keyboard_key_repeat_add_event(
    sb_event_dispatcher_t *event_dispatcher, sb_event_t *event)
{
    sb_repeat_event_t *repeat_event = sb_repeat_event_new(event);
    sb_log_debug("_add_event - Add: %d\n", event->keyboard.keycode);

    uint64_t now = sb_time_now_milliseconds();
    repeat_event->time = now;

    {
        if (sb_list_length(event_dispatcher->keyboard_key_repeat.events) == 0) {
        } else {
            sb_repeat_event_t *repeat_event = sb_list_at(event_dispatcher->keyboard_key_repeat.events, 0);
            sb_log_debug(" === First keycode: %d\n", repeat_event->event->keyboard.keycode);
        }
    }
    sb_list_push(event_dispatcher->keyboard_key_repeat.events, repeat_event);
    // DEBUG //
    // _print_repeat_event_list(event_dispatcher->keyboard_key_repeat.events);
    // DEBUG END //
}

void sb_event_dispatcher_keyboard_key_repeat_remove_event(
    sb_event_dispatcher_t *event_dispatcher, sb_event_t *event)
{
    sb_list_t *event_list = event_dispatcher->keyboard_key_repeat.events;

    // Do not remove initial release event.
    if (sb_list_length(event_list) == 0) {
        return;
    }

    sb_log_debug("_remove_event\n");
    // _print_repeat_event_list(event_list);
    for (int i = 0; i < sb_list_length(event_list); ++i) {
        sb_repeat_event_t *repeat_event = sb_list_at(event_list, i);
        if (repeat_event->event->keyboard.keycode == event->keyboard.keycode) {
            // Remove the repeat event info from the list and free it.
            sb_list_remove(event_list, i);
            sb_log_debug("Remove: %d\n", event->keyboard.keycode);
            sb_repeat_event_free(repeat_event);
            return;
        }
    }
    sb_log_warn("Not removed!\n");
}

bool sb_event_dispatcher_timer_has_event(
    sb_event_dispatcher_t *event_dispatcher)
{
    return sb_list_length(event_dispatcher->timer.events) > 0;
}

uint32_t sb_event_dispatcher_timer_add_event(
    sb_event_dispatcher_t *event_dispatcher, sb_event_t *event)
{
    sb_list_t *timer_events = event_dispatcher->timer.events;
    uint32_t new_id = 0;
    for (uint64_t i = 0; i < sb_list_length(timer_events); ++i) {
        sb_event_t *iter = sb_list_at(timer_events, i);
        if (new_id < iter->timer.id) {
            new_id = iter->timer.id + 1;
        }
    }
    event->timer.id = new_id;

    uint64_t now = sb_time_now_milliseconds();
    event->timer.time = now;

    sb_list_push(event_dispatcher->timer.events, event);

    return new_id;
}

void sb_event_dispatcher_timer_remove_event(
    sb_event_dispatcher_t *event_dispatcher, uint32_t id)
{
    sb_list_t *event_list = event_dispatcher->timer.events;

    for (uint64_t i = 0; i < sb_list_length(event_list); ++i) {
        sb_event_t *timer_event = sb_list_at(event_list, i);
        if (timer_event->timer.id == id) {
            sb_list_remove(event_list, i);
            sb_event_free(timer_event);
            break;
        }
    }
}

#ifdef __cplusplus
}
#endif
