#ifndef _SWINGBY_HELPERS_APPLICATION_H
#define _SWINGBY_HELPERS_APPLICATION_H

#include <wayland-protocols/staging/cursor-shape-v1.h>

enum sb_cursor_shape;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/// \brief Swingby cursor shape to `wp_cursor_shape_device_v1_shape`.
enum wp_cursor_shape_device_v1_shape
_to_wp_cursor_shape(enum sb_cursor_shape shape);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* _SWINGBY_HELPERS_APPLICATION_H */
