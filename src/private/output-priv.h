#ifndef SWINGBY_PRIVATE_OUTPUT_PRIV_H
#define SWINGBY_PRIVATE_OUTPUT_PRIV_H

#ifdef __cplusplus
extern "C" {
#endif

struct wl_output;

typedef struct sb_output_priv_t sb_output_priv_t;

typedef struct sb_application_t sb_application_t;
typedef struct sb_list_t sb_list_t;

struct sb_output_priv_t {
    sb_application_t *sb_application;
    sb_list_t *outputs;
};

void sb_output_priv_init(sb_output_priv_t *output_priv);

void sb_output_priv_add_listener(sb_output_priv_t *output_priv,
                                 struct wl_output *wl_output);

#ifdef __cplusplus
}
#endif

#endif /* SWINGBY_PRIVATE_OUTPUT_PRIV_H */
