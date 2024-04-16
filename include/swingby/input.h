#ifndef _FOUNDATION_INPUT_H
#define _FOUNDATION_INPUT_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum sb_pointer_button {
    SB_POINTER_BUTTON_NONE      = 0,
    SB_POINTER_BUTTON_LEFT      = 1,
    SB_POINTER_BUTTON_RIGHT     = 2,
    SB_POINTER_BUTTON_MIDDLE    = 4,
    SB_POINTER_BUTTON_UNIMPLEMENTED = 65536,
} sb_pointer_button;

typedef sb_pointer_button sb_pointer_button_flags;

#ifdef __cplusplus
}
#endif

#endif /* _FOUNDATION_INPUT_H */
