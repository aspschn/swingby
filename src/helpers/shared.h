#ifndef _SWINGBY_HELPERS_SHARED_H
#define _SWINGBY_HELPERS_SHARED_H

#include <swingby/common.h>

typedef struct sb_list_t sb_list_t;
enum sb_event_type;
typedef struct sb_event_t sb_event_t;
typedef struct sb_view_t sb_view_t;
typedef struct sb_point_t sb_point_t;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void _event_listener_filter_for_each(sb_list_t *listeners,
                                     enum sb_event_type type,
                                     sb_event_t *event);

/// \brief Find most child view of the root view.
///
/// Caution: It changes the position!
sb_view_t* _find_most_child(sb_view_t *view,
                            sb_point_t *position);

typedef float (*rounding_function_t)(float);
rounding_function_t _rounding_function(enum sb_rounding_policy policy);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* _SWINGBY_HELPERS_SHARED_H */
