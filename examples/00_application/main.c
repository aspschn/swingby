#include <stdio.h>

#include <swingby/application.h>

#include <wayland-client.h>

static void ext_global_handler(void *data,
                               struct wl_registry *wl_registry,
                               uint32_t name,
                               const char *interface,
                               uint32_t version)
{
    printf(" - interface: %s, wl_registry: %p\n", interface, wl_registry);
}

static void ext_global_remove_handler(void *data,
                                      struct wl_registry *wl_registry,
                                      uint32_t name)
{
    //
}

static const struct wl_registry_listener ext_registry_listener = {
    .global = ext_global_handler,
    .global_remove = ext_global_remove_handler,
};

int main(int argc, char *argv[])
{
    sb_application_t *app = sb_application_new(argc, argv);

    struct wl_registry *reg_ext = sb_application_get_registry_extension(app,
        &ext_registry_listener, NULL);

    wl_display_dispatch(sb_application_wl_display(app));
    wl_display_roundtrip(sb_application_wl_display(app));

    return sb_application_exec(app);
}
