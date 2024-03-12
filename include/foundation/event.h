#ifndef _FOUNDATION_EVENT_H
#define _FOUNDATION_EVENT_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum ft_event_target_type {
    FT_EVENT_TARGET_TYPE_APPLICATION,
    FT_EVENT_TARGET_TYPE_SURFACE,
    FT_EVENT_TARGET_TYPE_VIEW,
} ft_event_target_type;

typedef enum ft_event_type {
    FT_EVENT_TYPE_POINTER_MOVE,
    FT_EVENT_TYPE_REQUEST_UPDATE,
} ft_event_type;

typedef struct ft_event_t ft_event_t;

ft_event_t* ft_event_new(ft_event_target_type target_type,
                         void *target,
                         ft_event_type type);

#ifdef __cplusplus
}
#endif

#endif /* _FOUNDATION_EVENT_H */
