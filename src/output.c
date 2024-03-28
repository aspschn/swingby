#include <foundation/output.h>

#include <stdlib.h>

#include <wayland-client.h>

#ifdef __cplusplus
extern "C" {
#endif

struct ft_output_t {
    struct wl_output *wl_output;
};

ft_output_t* ft_output_new(struct wl_output *wl_output)
{
    ft_output_t *output = malloc(sizeof(ft_output_t));

    output->wl_output = wl_output;

    return output;
}

struct wl_output* ft_output_wl_output(ft_output_t *output)
{
    return output->wl_output;
}

#ifdef __cplusplus
}
#endif
