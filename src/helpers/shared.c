#include "shared.h"

#include <stddef.h>

#include <swingby/list.h>
#include <swingby/event.h>
#include <swingby/view.h>
#include <swingby/point.h>

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

sb_view_t* _find_most_child(sb_view_t *view,
                            sb_point_t *position)
{
    sb_list_t *children = sb_view_children(view);

    if (sb_list_length(children) == 0) {
        return view;
    }
    sb_view_t *child = sb_view_child_at(view, position);

    if (child == NULL) {
        return view;
    }

    position->x = position->x - sb_view_geometry(child)->pos.x;
    position->y = position->y - sb_view_geometry(child)->pos.y;

    return _find_most_child(child, position);
}
