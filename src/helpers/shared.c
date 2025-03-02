#include "shared.h"

#include <swingby/list.h>
#include <swingby/event.h>

void _event_listener_filter_for_each(sb_list_t *listeners,
                                     enum sb_event_type type,
                                     sb_event_t *event)
{
    uint64_t length = sb_list_length(listeners);
    for (uint64_t i = 0; i < length; ++i) {
        sb_event_listener_tuple_t *tuple = sb_list_at(listeners, i);
        if (tuple->type == type) {
            tuple->listener(event);
        }
    }
}
