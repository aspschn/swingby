#ifndef _FOUNDATION_COLOR_H
#define _FOUNDATION_COLOR_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct sb_color_t {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} sb_color_t;

#ifdef __cplusplus
}
#endif

#endif /* _FOUNDATION_COLOR_H */
