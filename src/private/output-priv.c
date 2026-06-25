#include "output-priv.h"

#include <wayland-client.h>

#include <swingby/output.h>
#include <swingby/list.h>
#include <swingby/log.h>
#include <swingby/application.h>

//!<====================
//!< Helper Functions
//!<====================

/// \brief Get the output object.
static sb_output_t* _get_output(sb_output_priv_t *output_priv,
                                struct wl_output *wl_output)
{
    sb_output_t *ret = NULL;

    for (int i = 0; i < sb_list_length(output_priv->outputs); ++i) {
        sb_output_t *output = sb_list_at(output_priv->outputs, i);
        if (sb_output_wl_output(output) == wl_output) {
            ret = output;
            break;
        }
    }

    return ret;
}

//!<===========
//!< Output
//!<===========

static void output_geometry_handler(void *data,
                                    struct wl_output *wl_output,
                                    int32_t x,
                                    int32_t y,
                                    int32_t physical_width,
                                    int32_t physical_height,
                                    int32_t subpixel,
                                    const char *make,
                                    const char *model,
                                    int32_t transform)
{
    sb_log_debug("output_geometry_handler() - %p\n", wl_output);
    sb_log_debug(" - make: %s\n", make);
    sb_log_debug(" - model: %s\n", model);
}

static void output_mode_handler(void *data,
                                struct wl_output *wl_output,
                                uint32_t flags,
                                int32_t width,
                                int32_t height,
                                int32_t refresh)
{
    sb_log_debug("output_mode_handler() - %p\n", wl_output);
}

static void output_done_handler(void *data,
                                struct wl_output *wl_output)
{
    sb_output_priv_t *output_priv = (sb_output_priv_t*)data;
    sb_output_t *output = _get_output(output_priv, wl_output);
    if (output == NULL) {
        sb_log_warn("output_done_handler() - output is NULL!\n");
        return;
    }
    sb_output_set_done(output, true);
}

static void output_scale_handler(void *data,
                                 struct wl_output *wl_output,
                                 int32_t factor)
{
    sb_output_priv_t *output_priv = (sb_output_priv_t*)data;
    sb_output_t *output = _get_output(output_priv, wl_output);
    if (output == NULL) {
        sb_log_warn("output_scale_handler() - output is NULL!\n");
        return;
    }
    sb_output_set_scale(output, factor);
}

static void output_name_handler(void *data,
                                struct wl_output *wl_output,
                                const char *name)
{
    sb_output_priv_t *output_priv = (sb_output_priv_t*)data;
    sb_output_t *output = _get_output(output_priv, wl_output);
    if (output == NULL) {
        sb_log_warn("output_name_handler() - output is NULL!\n");
        return;
    }
    sb_output_set_name(output, name);
}

static void output_description_handler(void *data,
                                       struct wl_output *wl_output,
                                       const char *description)
{
    sb_log_debug("outout_description_handler() - %p\n", wl_output);
}

static const struct wl_output_listener output_listener = {
    .geometry = output_geometry_handler,
    .mode = output_mode_handler,
    .done = output_done_handler,
    .scale = output_scale_handler,
    .name = output_name_handler,
    .description = output_description_handler,
};


void sb_output_priv_init(sb_output_priv_t *output_priv)
{
    output_priv->outputs = sb_list_new();
}

void sb_output_priv_add_listener(sb_output_priv_t *output_priv,
                                 struct wl_output *wl_output)
{
    wl_output_add_listener(wl_output, &output_listener, (void*)output_priv);
}
