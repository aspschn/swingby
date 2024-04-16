#include <swingby/event.h>

#include <stdlib.h>

#include <swingby/log.h>

#ifdef __cplusplus
extern "C" {
#endif

//!<=============
//!< Event
//!<=============

sb_event_t* sb_event_new(enum sb_event_target_type target_type,
                         void *target,
                         enum sb_event_type type)
{
    sb_event_t *event = malloc(sizeof(sb_event_t));

    event->target_type = target_type;
    event->target = target;
    event->type = type;
    event->propagation = true;

    return event;
}

enum sb_event_target_type sb_event_target_type(sb_event_t *event)
{
    return event->target_type;
}

void* sb_event_target(sb_event_t *event)
{
    return event->target;
}

enum sb_event_type sb_event_type(sb_event_t *event)
{
    return event->type;
}

bool sb_event_propagation(sb_event_t *event)
{
    return event->propagation;
}

void sb_event_set_propagation(sb_event_t *event, bool value)
{
    event->propagation = value;
}

const sb_size_t* sb_event_resize_old_size(sb_event_t *event)
{
    if (event->type != SB_EVENT_TYPE_RESIZE) {
        sb_log_warn("Trying to access a resize info, but the event type is not SB_EVENT_TYPE_RESIZE\n");
    }

    return &event->resize.old_size;
}

const sb_size_t* sb_event_resize_size(sb_event_t *event)
{
    if (event->type != SB_EVENT_TYPE_RESIZE) {
        sb_log_warn("Trying to access a resize info, but the event type is not SB_EVENT_TYPE_RESIZE\n");
    }

    return &event->resize.size;
}

//!<================
//!< Pointer Event
//!<================

sb_event_t* sb_pointer_event_new(enum sb_event_target_type target_type,
                                 void *target,
                                 enum sb_event_type type,
                                 sb_pointer_button button,
                                 const sb_point_t *position)
{
    sb_event_t *event = sb_event_new(target_type, target, type);

    event->pointer.button = button;
    event->pointer.position = *position;

    return event;
}

//!<=====================
//!< Event Listener
//!<=====================

sb_event_listener_tuple_t*
sb_event_listener_tuple_new(enum sb_event_type type,
                            void (*listener)(sb_event_t*))
{
    sb_event_listener_tuple_t *tuple = malloc(
        sizeof(sb_event_listener_tuple_t));

    tuple->type = type;
    tuple->listener = listener;

    return tuple;
}

void sb_event_listener_tuple_free(sb_event_listener_tuple_t *tuple)
{
    free(tuple);
}

#ifdef __cplusplus
}
#endif
