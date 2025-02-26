#ifndef _SWINGBY_XKB_XKB_CONTEXT_H
#define _SWINGBY_XKB_XKB_CONTEXT_H

#include <stdbool.h>

#include <xkbcommon/xkbcommon.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef struct sb_xkb_context_t {
    struct xkb_context *xkb_context;
    struct xkb_keymap *xkb_keymap;
    struct xkb_state *xkb_state;
} sb_xkb_context_t;

sb_xkb_context_t* sb_xkb_context_new(const char *string);

bool sb_xkb_context_state_is_mod_name_active(const sb_xkb_context_t *context,
                                             const char *mod_name);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* _SWINGBY_XKB_XKB_CONTEXT_H */
