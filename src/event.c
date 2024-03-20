#include <foundation/event.h>

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

//!<=============
//!< Event
//!<=============

ft_event_t* ft_event_new(enum ft_event_target_type target_type,
                         void *target,
                         enum ft_event_type type)
{
    ft_event_t *event = malloc(sizeof(ft_event_t));

    event->target_type = target_type;
    event->target = target;
    event->type = type;

    return event;
}

enum ft_event_target_type ft_event_target_type(ft_event_t *event)
{
    return event->target_type;
}

void* ft_event_target(ft_event_t *event)
{
    return event->target;
}

enum ft_event_type ft_event_type(ft_event_t *event)
{
    return event->type;
}

//!<================
//!< Pointer Event
//!<================

ft_event_t* ft_pointer_event_new(enum ft_event_target_type target_type,
                                 void *target,
                                 enum ft_event_type type,
                                 ft_pointer_button button,
                                 const ft_point_t *position)
{
    ft_event_t *event = ft_event_new(target_type, target, type);

    event->pointer.button = button;
    event->pointer.position = *position;

    return event;
}

//!<=====================
//!< Event Listener
//!<=====================

ft_event_listener_tuple_t*
ft_event_listener_tuple_new(enum ft_event_type type,
                            void (*listener)(ft_event_t*))
{
    ft_event_listener_tuple_t *tuple = malloc(
        sizeof(ft_event_listener_tuple_t));

    tuple->type = type;
    tuple->listener = listener;

    return tuple;
}

void ft_event_listener_tuple_free(ft_event_listener_tuple_t *tuple)
{
    free(tuple);
}

#ifdef __cplusplus
}
#endif
