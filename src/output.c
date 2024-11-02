#include <swingby/output.h>

#include <stdlib.h>

#include <wayland-client.h>

#include <swingby/log.h>

#ifdef __cplusplus
extern "C" {
#endif

struct sb_output_t {
    struct wl_output *wl_output;
    /// Wayland numeric name from the global registry handler.
    uint32_t wl_name;
    const char *name;
    const char *description;
    uint32_t scale;
    bool done;
};

sb_output_t* sb_output_new(struct wl_output *wl_output, uint32_t wl_name)
{
    sb_output_t *output = malloc(sizeof(sb_output_t));

    output->wl_output = wl_output;
    output->wl_name = wl_name;

    output->done = false;

    return output;
}

struct wl_output* sb_output_wl_output(sb_output_t *output)
{
    return output->wl_output;
}

uint32_t sb_output_wl_name(sb_output_t *output)
{
    return output->wl_name;
}

const char* sb_output_name(const sb_output_t *output)
{
    return output->name;
}

void sb_output_set_name(sb_output_t *output, const char *name)
{
    output->name = name;
}

uint32_t sb_output_scale(const sb_output_t *output)
{
    return output->scale;
}

void sb_output_set_scale(sb_output_t *output, uint32_t scale)
{
    output->scale = scale;
}

bool sb_output_done(const sb_output_t *output)
{
    return output->done;
}

void sb_output_set_done(sb_output_t *output, bool value)
{
    output->done = value;
    if (value == true) {
        sb_log_debug("sb_output_set_done() - true. Done.\n");
        sb_log_debug(" - name: %s\n", sb_output_name(output));
        sb_log_debug(" - scale: %d\n", sb_output_scale(output));
    }
}

#ifdef __cplusplus
}
#endif
