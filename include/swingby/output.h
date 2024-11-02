#ifndef _FOUNDATION_OUTPUT_H
#define _FOUNDATION_OUTPUT_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct wl_output;

typedef struct sb_output_t sb_output_t;

sb_output_t* sb_output_new(struct wl_output *wl_output, uint32_t wl_name);

struct wl_output* sb_output_wl_output(sb_output_t *output);

uint32_t sb_output_wl_name(sb_output_t *output);

const char* sb_output_name(const sb_output_t *output);

void sb_output_set_name(sb_output_t *output, const char *name);

uint32_t sb_output_scale(const sb_output_t *output);

void sb_output_set_scale(sb_output_t *output, uint32_t scale);

bool sb_output_done(const sb_output_t *output);

void sb_output_set_done(sb_output_t *output, bool value);

#ifdef __cplusplus
}
#endif

#endif /* _FOUNDATION_OUTPUT_H */
