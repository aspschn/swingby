#ifndef _FOUNDATION_OUTPUT_H
#define _FOUNDATION_OUTPUT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct wl_output;

typedef struct ft_output_t ft_output_t;

ft_output_t* ft_output_new(struct wl_output *wl_output, uint32_t name);

struct wl_output* ft_output_wl_output(ft_output_t *output);

uint32_t ft_output_name(ft_output_t *output);

#ifdef __cplusplus
}
#endif

#endif /* _FOUNDATION_OUTPUT_H */
