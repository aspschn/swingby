#include "xkb-context.h"

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

sb_xkb_context_t* sb_xkb_context_new(const char *string)
{
    sb_xkb_context_t *xkb_context = malloc(sizeof(sb_xkb_context_t));

    xkb_context->xkb_context = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
    xkb_context->xkb_keymap = xkb_keymap_new_from_string(
        xkb_context->xkb_context, string,
        XKB_KEYMAP_FORMAT_TEXT_V1,
        XKB_KEYMAP_COMPILE_NO_FLAGS);
    xkb_context->xkb_state = xkb_state_new(xkb_context->xkb_keymap);

    return xkb_context;
}

bool sb_xkb_context_state_is_mod_name_active(const sb_xkb_context_t *context,
                                             const char *mod_name)
{
    int ret = xkb_state_mod_name_is_active(context->xkb_state, mod_name,
        XKB_STATE_MODS_DEPRESSED);
    return (ret == 1) ? true : false;
}

#ifdef __cplusplus
}
#endif // __cplusplus
