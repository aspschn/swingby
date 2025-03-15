#ifndef _FOUNDATION_EVENT_DISPATCHER_H
#define _FOUNDATION_EVENT_DISPATCHER_H

#include <stdbool.h>
#include <stdint.h>

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

void sb_event_dispatcher_keyboard_key_repeat_set_delay(
    sb_event_dispatcher_t *event_dispatcher, uint32_t delay);

void sb_event_dispatcher_keyboard_key_repeat_set_rate(
    sb_event_dispatcher_t *event_dispatcher, uint32_t rate);

/// \brief Set the event to repeat.
///
/// Pass NULL to event to stop repeat.
void sb_event_dispatcher_keyboard_key_repeat_set_event(
    sb_event_dispatcher_t *event_dispatcher, sb_event_t *event);

bool sb_event_dispatcher_keyboard_key_repeat_has_event(
    sb_event_dispatcher_t *event_dispatcher);

void sb_event_dispatcher_keyboard_key_repeat_add_event(
    sb_event_dispatcher_t *event_dispatcher, sb_event_t *event);

void sb_event_dispatcher_keyboard_key_repeat_remove_event(
    sb_event_dispatcher_t *event_dispatcher, sb_event_t *event);

bool sb_event_dispatcher_timer_has_event(
    sb_event_dispatcher_t *event_dispatcher);

/// \brief Add timer event and set id and return.
uint32_t sb_event_dispatcher_timer_add_event(
    sb_event_dispatcher_t *event_dispatcher, sb_event_t *event);

void sb_event_dispatcher_timer_remove_event(
    sb_event_dispatcher_t *event_dispatcher, uint32_t id);

#ifdef __cplusplus
}
#endif

#endif /* _FOUNDATION_EVENT_DISPATCHER_H */
