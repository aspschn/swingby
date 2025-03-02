#ifndef _SWINGBY_HELPERS_SHARED_H
#define _SWINGBY_HELPERS_SHARED_H

typedef struct sb_list_t sb_list_t;
enum sb_event_type;
typedef struct sb_event_t sb_event_t;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void _event_listener_filter_for_each(sb_list_t *listeners,
                                     enum sb_event_type type,
                                     sb_event_t *event);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* _SWINGBY_HELPERS_SHARED_H */
