#ifndef _FOUNDATION_EVENT_DISPATCHER_H
#define _FOUNDATION_EVENT_DISPATCHER_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct sb_event_t sb_event_t;

//!<==========
//!< Queue
//!<==========

typedef struct sb_queue_t sb_queue_t;

sb_queue_t* sb_queue_new();

void sb_queue_enqueue(sb_queue_t *queue, void *data);

void* sb_queue_dequeue(sb_queue_t *queue);

//!<=====================
//!< Event Dispatcher
//!<=====================

typedef struct sb_event_dispatcher_t sb_event_dispatcher_t;

sb_event_dispatcher_t* sb_event_dispatcher_new();

void sb_event_dispatcher_post_event(sb_event_dispatcher_t *event_dispatcher,
                                    sb_event_t *event);

void
sb_event_dispatcher_process_events(sb_event_dispatcher_t *event_dispatcher);

#ifdef __cplusplus
}
#endif

#endif /* _FOUNDATION_EVENT_DISPATCHER_H */
