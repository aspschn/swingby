#ifndef SWINGBY_POPUP_H
#define SWINGBY_POPUP_H

#include <stdbool.h>

#include <swingby/common.h>
#include <swingby/point.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef struct sb_surface_role_header_t sb_surface_role_header_t;

typedef struct sb_surface_t sb_surface_t;

/// \struct sb_popup_t
typedef struct sb_popup_t sb_popup_t;

SB_EXPORT
sb_popup_t* sb_popup_new();

SB_EXPORT
void sb_popup_set_parent(sb_popup_t *popup, sb_surface_role_header_t *parent);

SB_EXPORT
sb_surface_t* sb_popup_surface(sb_popup_t *popup);

SB_EXPORT
sb_point_i_t sb_popup_position(const sb_popup_t *popup);

SB_EXPORT
void sb_popup_set_position(sb_popup_t *popup, sb_point_i_t position);

/// \memberof sb_popup_t
/// \brief Set grabbable property for popup surface.
SB_EXPORT
void sb_popup_set_grabbable(sb_popup_t *popup, bool value);

SB_EXPORT
void sb_popup_grab_for_button(sb_popup_t *popup);

SB_EXPORT
void sb_popup_grab_for_key(sb_popup_t *popup);

SB_EXPORT
void sb_popup_show(sb_popup_t *popup);

SB_EXPORT
void sb_popup_hide(sb_popup_t *popup);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* SWINGBY_POPUP_H */
