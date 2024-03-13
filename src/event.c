#include <foundation/event.h>

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

//!<=============
//!< Event
//!<=============

ft_event_t* ft_event_new(ft_event_target_type target_type,
                         void *target,
                         ft_event_type type)
{
    ft_event_t *event = malloc(sizeof(ft_event_t));

    event->target_type = target_type;
    event->target = target;
    event->type = type;

    return event;
}

ft_event_target_type ft_event_event_target_type(ft_event_t *event)
{
    return event->target_type;
}

void* ft_event_event_target(ft_event_t *event)
{
    return event->target;
}

ft_event_type ft_event_event_type(ft_event_t *event)
{
    return event->type;
}

//!<================
//!< Pointer Event
//!<================

ft_event_t* ft_pointer_event_new(ft_event_target_type target_type,
                                 void *target,
                                 ft_event_type type,
                                 ft_pointer_button button,
                                 const ft_point_t *position)
{
    ft_event_t *event = malloc(sizeof(ft_event_t));

    event->pointer.button = button;
    event->pointer.position = *position;

    return event;
}

#ifdef __cplusplus
}
#endif
