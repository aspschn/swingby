#ifndef _FOUNDATION_INPUT_H
#define _FOUNDATION_INPUT_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum ft_pointer_button {
    FT_POINTER_BUTTON_NONE      = 0,
    FT_POINTER_BUTTON_LEFT      = 1,
    FT_POINTER_BUTTON_RIGHT     = 2,
    FT_POINTER_BUTTON_MIDDLE    = 4,
    FT_POINTER_BUTTON_UNIMPLEMENTED = 65536,
} ft_pointer_button;

typedef ft_pointer_button ft_pointer_button_flags;

#ifdef __cplusplus
}
#endif

#endif /* _FOUNDATION_INPUT_H */
