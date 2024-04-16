#include <swingby/output.h>

#include <stdlib.h>

#include <wayland-client.h>

#ifdef __cplusplus
extern "C" {
#endif

struct sb_output_t {
    struct wl_output *wl_output;
    uint32_t name;
};

sb_output_t* sb_output_new(struct wl_output *wl_output, uint32_t name)
{
    sb_output_t *output = malloc(sizeof(sb_output_t));

    output->wl_output = wl_output;
    output->name = name;

    return output;
}

struct wl_output* sb_output_wl_output(sb_output_t *output)
{
    return output->wl_output;
}

uint32_t sb_output_name(sb_output_t *output)
{
    return output->name;
}

#ifdef __cplusplus
}
#endif
