#ifndef _FOUNDATION_EVENT_H
#define _FOUNDATION_EVENT_H

#include <foundation/input.h>
#include <foundation/point.h>
#include <foundation/size.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum ft_event_target_type {
    FT_EVENT_TARGET_TYPE_APPLICATION,
    FT_EVENT_TARGET_TYPE_SURFACE,
    FT_EVENT_TARGET_TYPE_VIEW,
} ft_event_target_type;

typedef enum ft_event_type {
    FT_EVENT_TYPE_POINTER_ENTER,
    FT_EVENT_TYPE_POINTER_LEAVE,
    FT_EVENT_TYPE_POINTER_MOVE,
    FT_EVENT_TYPE_POINTER_PRESS,
    FT_EVENT_TYPE_POINTER_RELEASE,
    FT_EVENT_TYPE_POINTER_CLICK,
    FT_EVENT_TYPE_REQUEST_UPDATE,
    FT_EVENT_TYPE_MOVE,
    FT_EVENT_TYPE_RESIZE,
} ft_event_type;

typedef struct ft_event_t ft_event_t;

typedef struct ft_event_listener_tuple_t {
    ft_event_type type;
    void (*listener)(ft_event_t*);
} ft_event_listener_tuple_t;

typedef struct ft_pointer_event_t {
    ft_pointer_button button;
    ft_point_t position;
} ft_pointer_event_t;

typedef struct ft_move_event_t {
    ft_point_t old_position;
    ft_point_t position;
} ft_move_event_t;

typedef struct ft_resize_event_t {
    ft_size_t old_size;
    ft_size_t size;
} ft_resize_event_t;

struct ft_event_t {
    ft_event_target_type target_type;
    void *target;
    ft_event_type type;
    union {
        ft_pointer_event_t pointer;
        ft_move_event_t move;
        ft_resize_event_t resize;
    };
};

ft_event_t* ft_event_new(ft_event_target_type target_type,
                         void *target,
                         ft_event_type type);

ft_event_target_type ft_event_event_target_type(ft_event_t *event);

void* ft_event_event_target(ft_event_t *event);

ft_event_type ft_event_event_type(ft_event_t *event);

void ft_event_free(ft_event_t *event);

//!<=================
//!< Pointer Event
//!<=================

ft_event_t* ft_pointer_event_new(ft_event_target_type target_type,
                                 void *target,
                                 ft_event_type type,
                                 ft_pointer_button button,
                                 const ft_point_t *position);

ft_pointer_button ft_pointer_event_button(ft_pointer_event_t *event);

//!<=====================
//!< Event Listener
//!<=====================

ft_event_listener_tuple_t*
ft_event_listener_tuple_new(ft_event_type type,
                            void (*listener)(ft_event_t*));

void ft_event_listener_tuple_free(ft_event_listener_tuple_t *tuple);

#ifdef __cplusplus
}
#endif

#endif /* _FOUNDATION_EVENT_H */
