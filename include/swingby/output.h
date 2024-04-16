#ifndef _FOUNDATION_OUTPUT_H
#define _FOUNDATION_OUTPUT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct wl_output;

typedef struct sb_output_t sb_output_t;

sb_output_t* sb_output_new(struct wl_output *wl_output, uint32_t name);

struct wl_output* sb_output_wl_output(sb_output_t *output);

uint32_t sb_output_name(sb_output_t *output);

#ifdef __cplusplus
}
#endif

#endif /* _FOUNDATION_OUTPUT_H */
