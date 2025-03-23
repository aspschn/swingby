#ifndef _FOUNDATION_EVENT_H
#define _FOUNDATION_EVENT_H

#include <stdbool.h>

#include <swingby/input.h>
#include <swingby/point.h>
#include <swingby/size.h>

#ifdef __cplusplus
extern "C" {
#endif

enum sb_event_target_type {
    SB_EVENT_TARGET_TYPE_APPLICATION        = 1,
    SB_EVENT_TARGET_TYPE_DESKTOP_SURFACE    = 2,
    SB_EVENT_TARGET_TYPE_SURFACE            = 3,
    SB_EVENT_TARGET_TYPE_VIEW               = 4,
};

enum sb_event_type {
    SB_EVENT_TYPE_POINTER_ENTER             = 10,
    SB_EVENT_TYPE_POINTER_LEAVE             = 11,
    SB_EVENT_TYPE_POINTER_MOVE              = 12,
    SB_EVENT_TYPE_POINTER_PRESS             = 13,
    SB_EVENT_TYPE_POINTER_RELEASE           = 14,
    SB_EVENT_TYPE_POINTER_CLICK             = 15,
    SB_EVENT_TYPE_POINTER_DOUBLE_CLICK      = 16,
    SB_EVENT_TYPE_POINTER_SCROLL            = 20,
    SB_EVENT_TYPE_KEYBOARD_ENTER            = 30,
    SB_EVENT_TYPE_KEYBOARD_LEAVE            = 31,
    SB_EVENT_TYPE_KEYBOARD_KEY_PRESS        = 32,
    SB_EVENT_TYPE_KEYBOARD_KEY_RELEASE      = 33,
    SB_EVENT_TYPE_REQUEST_UPDATE            = 70,
    SB_EVENT_TYPE_SHOW                      = 80,
    SB_EVENT_TYPE_HIDE                      = 81,
    /// \brief View position moved.
    SB_EVENT_TYPE_MOVE                      = 100,
    /// \brief Desktop surface, surface and view size cahnged.
    SB_EVENT_TYPE_RESIZE                    = 101,
    SB_EVENT_TYPE_STATE_CHANGE              = 110,
    /// \brief Preferred buffer scale changed for surface.
    SB_EVENT_TYPE_PREFERRED_SCALE           = 130,
    SB_EVENT_TYPE_TIMEOUT                   = 140,
};

typedef struct sb_event_t sb_event_t;

typedef struct sb_event_listener_tuple_t {
    enum sb_event_type type;
    void (*listener)(sb_event_t*);
} sb_event_listener_tuple_t;

/// \brief Pointer enter, leave and move event.
typedef struct sb_pointer_event_t {
    sb_pointer_button button;
    sb_point_t position;
} sb_pointer_event_t;

/// \brief This should not be used.
///
/// Pointer move event must use `sb_pointer_event_t`.
typedef struct sb_move_event_t {
    sb_point_t old_position;
    sb_point_t position;
} sb_move_event_t;

typedef struct sb_resize_event_t {
    sb_size_t old_size;
    sb_size_t size;
} sb_resize_event_t;

/// \brief An event used when toplevel desktop surface state changed.
typedef struct sb_state_change_event_t {
    /// Must a valid value in `enum sb_desktop_surface_toplevel_state`.
    int state;
    /// The value of the changed state. On or off.
    bool value;
    sb_size_t size;
} sb_state_change_event_t;

/// \brief Keyboard enter, leave, key press and key release.
typedef struct sb_keyboard_event_t {
    int key;
    uint32_t keycode;
    bool repeated;
} sb_keyboard_event_t;

typedef struct sb_scale_event_t {
    uint32_t scale;
} sb_scale_event_t;

typedef struct sb_timer_event_t {
    uint32_t id;
    uint32_t interval;
    bool repeat;
    /// Internal use.
    uint64_t time;
} sb_timer_event_t;

typedef struct sb_scroll_event_t {
    enum sb_pointer_scroll_source source;
    enum sb_pointer_scroll_axis axis;
    float value;
    bool momentum;
} sb_scroll_event_t;

struct sb_event_t {
    enum sb_event_target_type target_type;
    void *target;
    enum sb_event_type type;
    bool propagation;
    union {
        sb_pointer_event_t pointer;
        sb_keyboard_event_t keyboard;
        sb_move_event_t move;
        sb_resize_event_t resize;
        sb_state_change_event_t state_change;
        sb_scale_event_t scale;
        sb_timer_event_t timer;
        sb_scroll_event_t scroll;
    };
};

sb_event_t* sb_event_new(enum sb_event_target_type target_type,
                         void *target,
                         enum sb_event_type type);

enum sb_event_target_type sb_event_target_type(sb_event_t *event);

void* sb_event_target(sb_event_t *event);

enum sb_event_type sb_event_type(sb_event_t *event);

bool sb_event_propagation(sb_event_t *event);

void sb_event_set_propagation(sb_event_t *event, bool value);

/// \brief Returns `event->resize.old_size`.
const sb_size_t* sb_event_resize_old_size(sb_event_t *event);

/// \brief Returns `event->resize.size`.
const sb_size_t* sb_event_resize_size(sb_event_t *event);

void sb_event_free(sb_event_t *event);

//!<=================
//!< Pointer Event
//!<=================

sb_event_t* sb_pointer_event_new(enum sb_event_target_type target_type,
                                 void *target,
                                 enum sb_event_type type,
                                 sb_pointer_button button,
                                 const sb_point_t *position);

sb_pointer_button sb_pointer_event_button(sb_pointer_event_t *event);

//!<=================
//!< Move Event
//!<=================

const sb_point_t* sb_event_move_old_position(sb_event_t *event);

const sb_point_t* sb_event_move_position(sb_event_t *event);

//!<======================
//!< State Change Event
//!<======================

int sb_event_state_change_state(sb_event_t *event);

bool sb_event_state_change_value(sb_event_t *event);

//!<=====================
//!< Event Listener
//!<=====================

sb_event_listener_tuple_t*
sb_event_listener_tuple_new(enum sb_event_type type,
                            void (*listener)(sb_event_t*));

void sb_event_listener_tuple_free(sb_event_listener_tuple_t *tuple);

#ifdef __cplusplus
}
#endif

#endif /* _FOUNDATION_EVENT_H */
