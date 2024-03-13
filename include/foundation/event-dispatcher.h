#ifndef _FOUNDATION_EVENT_DISPATCHER_H
#define _FOUNDATION_EVENT_DISPATCHER_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ft_event_t ft_event_t;

//!<==========
//!< Queue
//!<==========

typedef struct ft_queue_t ft_queue_t;

ft_queue_t* ft_queue_new();

void ft_queue_enqueue(ft_queue_t *queue, void *data);

void* ft_queue_dequeue(ft_queue_t *queue);

//!<=====================
//!< Event Dispatcher
//!<=====================

typedef struct ft_event_dispatcher_t ft_event_dispatcher_t;

ft_event_dispatcher_t* ft_event_dispatcher_new();

void ft_event_dispatcher_post_event(ft_event_dispatcher_t *event_dispatcher,
                                    ft_event_t *event);

void
ft_event_dispatcher_process_events(ft_event_dispatcher_t *event_dispatcher);

#ifdef __cplusplus
}
#endif

#endif /* _FOUNDATION_EVENT_DISPATCHER_H */
