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

enum sb_keyboard_modifier {
    SB_KEYBOARD_MODIFIER_NONE       = 0,
    SB_KEYBOARD_MODIFIER_SHIFT,
    /// \brief Same as Ctrl key.
    SB_KEYBOARD_MODIFIER_CONTROL,
    SB_KEYBOARD_MODIFIER_ALT,
    /// \brief The Super key is commonly located between ctrl and alt key.
    ///
    /// The Super key is often used interchangeably with the Meta key, but
    /// the Meta key is a different key from the Meta key.
    /// Therefore, the term "Super" is used to refer to this key.
    SB_KEYBOARD_MODIFIER_SUPER,
};

typedef enum sb_keyboard_modifier sb_keyboard_modifier_flags;

enum sb_pointer_scroll_source {
    SB_POINTER_SCROLL_SOURCE_WHEEL      = 0,
    SB_POINTER_SCROLL_SOURCE_FINGER     = 1,
    SB_POINTER_SCROLL_SOURCE_CONTINUOUS = 2,
    SB_POINTER_SCROLL_SOURCE_WHEEL_TILT = 3,
};

enum sb_pointer_scroll_axis {
    SB_POINTER_SCROLL_AXIS_VERTICAL_SCROLL      = 0,
    SB_POINTER_SCROLL_AXIS_HORIZONTAL_SCROLL    = 1,
};

#ifdef __cplusplus
}
#endif

#endif /* _FOUNDATION_INPUT_H */
