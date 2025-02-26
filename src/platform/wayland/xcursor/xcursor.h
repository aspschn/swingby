#ifndef _SWINGBY_XCURSOR_XCURSOR_H
#define _SWINGBY_XCURSOR_XCURSOR_H

#include <stdint.h>

#include <swingby/point.h>
#include <swingby/size.h>
#include <swingby/list.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef struct sb_xcursor_t {
    const char *name;
    sb_size_t size;
    sb_point_t hot_spot;
} sb_xcursor_t;

typedef struct sb_xcursor_theme_t {
    /// Directory name.
    const char *id;
    const char *name;
    sb_list_t *files;
} sb_xcursor_theme_t;

typedef struct sb_xcursor_theme_manager_t {
    sb_list_t *ids;
} sb_xcursor_theme_manager_t;

//!<================
//!< Theme Manager
//!<================

/// \brief Load Xcursor icon themes.
sb_xcursor_theme_manager_t* sb_xcursor_theme_manager_load();

const sb_list_t* sb_xcursor_theme_manager_ids(
    sb_xcursor_theme_manager_t *manager);

void sb_xcursor_theme_manager_free(sb_xcursor_theme_manager_t *manager);

//!<================
//!< Theme
//!<================

sb_xcursor_theme_t* sb_xcursor_theme_new(const char *id);

void sb_xcursor_theme_scan_files(sb_xcursor_theme_t *theme);

void sb_xcursor_theme_free(sb_xcursor_theme_t *theme);

//!<================
//!< Cursor
//!<================


#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* _SWINGBY_XCURSOR_XCURSOR_H */
