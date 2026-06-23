#include <swingby/application.h>

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <unistd.h>
#include <sys/mman.h>
#include <poll.h>

#include <linux/input.h>

#include <wayland-client.h>
#include <wayland-protocols/stable/xdg-shell.h>
#include <wayland-protocols/staging/cursor-shape-v1.h>
#include <wayland-protocols/unstable/text-input-unstable-v3.h>

#include <swingby/log.h>
#include <swingby/surface.h>
#include <swingby/desktop-surface.h>
#include <swingby/view.h>
#include <swingby/list.h>
#include <swingby/input.h>
#include <swingby/cursor.h>
#include <swingby/output.h>
#include <swingby/event.h>
#include <swingby/event-dispatcher.h>

#include "egl-context/egl-context.h"

#include "xkb/xkb-context.h"
#include "xcursor/xcursor.h"

#include "helpers/shared.h"
#include "helpers/application.h"

#include "private/pointer-priv.h"

struct sb_application_t {
    sb_egl_t *egl;
    /// `struct wl_display`.
    struct wl_display *wl_display;
    /// `struct wl_registry`.
    struct wl_registry *_wl_registry;
    struct wl_compositor *_wl_compositor;
    struct xdg_wm_base *_xdg_wm_base;
    struct wl_seat *_wl_seat;
    struct wl_pointer *_wl_pointer;
    struct wl_keyboard *_wl_keyboard;
    struct wl_touch *_wl_touch;
    struct wl_shm *wl_shm;
    struct zwp_text_input_manager_v3 *zwp_text_input_manager_v3;
    struct zwp_text_input_v3 *zwp_text_input_v3;
    sb_pointer_priv_t pointer;
    struct {
        /// \brief Current keyboard surface.
        sb_surface_t *surface;
    } keyboard;
    struct {
        struct wl_surface *wl_surface;
        const char *preedit_text;
        const char *commit_text;
    } text_input;
    struct sb_xkb_context_t *xkb_context;
    /// \brief List of the toplevel desktop surfaces.
    sb_list_t *toplevels;
    /// \brief List of the popup desktop surfaces.
    sb_list_t *popups;
    struct {
        sb_xcursor_theme_manager_t *manager;
        char current[256];
    } xcursor;
    /// \brief Output list.
    sb_list_t *outputs;
    /// \brief An event dispatcher.
    sb_event_dispatcher_t *event_dispatcher;
    sb_list_t *event_listeners;
};

// Singleton object.
static sb_application_t *_sb_application_instance = NULL;

//!<============
//!< Registry
//!<============

static void app_global_handler(void *data,
                               struct wl_registry *wl_registry,
                               uint32_t name,
                               const char *interface,
                               uint32_t version);

static void app_global_remove_handler(void *data,
                                      struct wl_registry *wl_registry,
                                      uint32_t name);

static const struct wl_registry_listener app_registry_listener = {
    .global = app_global_handler,
    .global_remove = app_global_remove_handler,
};

//!<=========
//!< XDG
//!<=========

static void xdg_wm_base_ping_handler(void *data,
                                     struct xdg_wm_base *xdg_wm_base,
                                     uint32_t serial);

static const struct xdg_wm_base_listener app_xdg_wm_base_listener = {
    .ping = xdg_wm_base_ping_handler,
};

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
                                       int32_t transform);

static void output_mode_handler(void *data,
                                struct wl_output *wl_output,
                                uint32_t flags,
                                int32_t width,
                                int32_t height,
                                int32_t refresh);

static void output_done_handler(void *data,
                                struct wl_output *wl_output);

static void output_scale_handler(void *data,
                                 struct wl_output *wl_output,
                                 int32_t factor);

static void output_name_handler(void *data,
                                struct wl_output *wl_output,
                                const char *name);

static void output_description_handler(void *data,
                                       struct wl_output *wl_output,
                                       const char *description);

static const struct wl_output_listener output_listener = {
    .geometry = output_geometry_handler,
    .mode = output_mode_handler,
    .done = output_done_handler,
    .scale = output_scale_handler,
    .name = output_name_handler,
    .description = output_description_handler,
};

//!<============
//!< Keyboard
//!<============

static void keyboard_keymap_handler(void *data,
                                    struct wl_keyboard *wl_keyboard,
                                    uint32_t format,
                                    int32_t fd,
                                    uint32_t size);

static void keyboard_enter_handler(void *data,
                                   struct wl_keyboard *wl_keyboard,
                                   uint32_t serial,
                                   struct wl_surface *wl_surface,
                                   struct wl_array *keys);

static void keyboard_leave_handler(void *data,
                                   struct wl_keyboard *wl_keyboard,
                                   uint32_t serial,
                                   struct wl_surface *wl_surface);

static void keyboard_key_handler(void *data,
                                 struct wl_keyboard *wl_keyboard,
                                 uint32_t serial,
                                 uint32_t time,
                                 uint32_t key,
                                 uint32_t state);

static void keyboard_modifiers_handler(void *data,
                                       struct wl_keyboard *wl_keyboard,
                                       uint32_t serial,
                                       uint32_t mods_depressed,
                                       uint32_t mods_latched,
                                       uint32_t mods_locked,
                                       uint32_t group);

static void keyboard_repeat_info_handler(void *data,
                                         struct wl_keyboard *wl_keyboard,
                                         int32_t rate,
                                         int32_t delay);

static const struct wl_keyboard_listener keyboard_listener = {
    .keymap = keyboard_keymap_handler,
    .enter = keyboard_enter_handler,
    .leave = keyboard_leave_handler,
    .key = keyboard_key_handler,
    .modifiers = keyboard_modifiers_handler,
    .repeat_info = keyboard_repeat_info_handler,
};

//!<=========
//!< Seat
//!==========

static void seat_capabilities_handler(void *data,
                                      struct wl_seat *wl_seat,
                                      uint32_t capabilities);

static void seat_name_handler(void *data,
                              struct wl_seat *wl_seat,
                              const char *name);

static const struct wl_seat_listener seat_listener = {
    .capabilities = seat_capabilities_handler,
    .name = seat_name_handler,
};

//!<====================
//!< Shm
//!<====================

static void shm_format_handler(void *data,
                               struct wl_shm *wl_shm,
                               uint32_t format);

static const struct wl_shm_listener shm_listener = {
    .format = shm_format_handler,
};

static void buffer_release_handler(void *data,
                                   struct wl_buffer *wl_buffer);

static const struct wl_buffer_listener buffer_listener = {
    .release = buffer_release_handler,
};

//!<====================
//!< Text Input
//!<====================

static void text_input_enter_handler(void *data,
                                     struct zwp_text_input_v3 *text_input,
                                     struct wl_surface *wl_surface);

static void text_input_leave_handler(void *data,
                                     struct zwp_text_input_v3 *text_input,
                                     struct wl_surface *wl_surface);

static void text_input_preedit_string_handler(void *data,
    struct zwp_text_input_v3 *text_input,
    const char *text,
    int32_t cursor_begin,
    int32_t cursor_end);

static void text_input_commit_string_handler(void *data,
    struct zwp_text_input_v3 *text_input,
    const char *text);

static void text_input_delete_surrounding_text_handler(void *data,
    struct zwp_text_input_v3 *zwp_text_input_v3,
    uint32_t before_length,
    uint32_t after_length);

static void text_input_done_handler(void *data,
                                    struct zwp_text_input_v3 *text_input,
                                    uint32_t serial);

static const struct zwp_text_input_v3_listener text_input_listener = {
    .enter = text_input_enter_handler,
    .leave = text_input_leave_handler,
    .preedit_string = text_input_preedit_string_handler,
    .commit_string = text_input_commit_string_handler,
    .delete_surrounding_text = text_input_delete_surrounding_text_handler,
    .done = text_input_done_handler,
};

//!<====================
//!< Helper Functions
//!<====================

/// \brief Find matching surface with wl_surface.
static sb_surface_t* _find_surface(sb_application_t *app,
                                   struct wl_surface *wl_surface)
{
    sb_surface_t *found = NULL;
    // Search toplevels.
    sb_list_t *list = app->toplevels;
    for (int i = 0; i < sb_list_length(list); ++i) {
        sb_desktop_surface_t *desktop_surface = sb_list_at(list, i);
        sb_surface_t *surface = sb_desktop_surface_surface(desktop_surface);
        if (sb_surface_wl_surface(surface) == wl_surface) {
            found = surface;
            break;
        }
    }
    if (found != NULL) {
        return found;
    }
    // Search popups.
    list = app->popups;
    for (int i = 0; i < sb_list_length(list); ++i) {
        sb_desktop_surface_t *desktop_surface = sb_list_at(list, i);
        sb_surface_t *surface = sb_desktop_surface_surface(desktop_surface);
        if (sb_surface_wl_surface(surface) == wl_surface) {
            found = surface;
            break;
        }
    }
    // Surface must not be null.
    if (found == NULL) {
        sb_log_error("_find_surface() - Not found surface for wl_surface: %p\n",
            wl_surface);
    }

    return found;
}

static void _post_pointer_enter_event(sb_view_t *view,
                                      float x,
                                      float y)
{
    sb_event_t *event = sb_event_new(SB_EVENT_TARGET_TYPE_VIEW,
        view,
        SB_EVENT_TYPE_POINTER_ENTER);

    event->pointer.button = SB_POINTER_BUTTON_NONE;
    event->pointer.position.x = x;
    event->pointer.position.y = y;

    sb_application_post_event(sb_application_instance(), event);
}

static void _post_pointer_leave_event(sb_view_t *view,
                                      float x,
                                      float y)
{
    sb_event_t *event = sb_event_new(SB_EVENT_TARGET_TYPE_VIEW,
        view,
        SB_EVENT_TYPE_POINTER_LEAVE);

    event->pointer.button = SB_POINTER_BUTTON_NONE;
    // TODO: How to get pointer leave position?
    event->pointer.position.x = x;
    event->pointer.position.y = y;

    sb_application_post_event(sb_application_instance(), event);
}

/// \brief What is this function's purpose?
bool _is_child_of(sb_view_t *view, sb_view_t *other)
{
    sb_list_t *children = sb_view_children(view);

    if (sb_list_length(children) == 0) {
        return false;
    }

    for (int i = 0; i < sb_list_length(children); ++i) {
        sb_view_t *child = sb_list_at(children, i);
        bool result = _is_child_of(child, other);
        if (result == true) {
            return true;
        }
    }

    return false;
}

/// \brief Get the output object.
static sb_output_t* _get_output(sb_application_t *application,
                                struct wl_output *wl_output)
{
    sb_output_t *ret = NULL;

    for (int i = 0; i < sb_list_length(application->outputs); ++i) {
        sb_output_t *output = sb_list_at(application->outputs, i);
        if (sb_output_wl_output(output) == wl_output) {
            ret = output;
            break;
        }
    }

    return ret;
}

//!<===============
//!< Application
//!<===============

sb_application_t* sb_application_new(int argc, char *argv[])
{
    sb_application_t *app = malloc(sizeof(sb_application_t));

    app->wl_display = wl_display_connect(NULL);

    // Null initializations.
    app->_wl_seat = NULL;
    app->_wl_pointer = NULL;
    app->_wl_keyboard = NULL;
    app->_wl_touch = NULL;
    app->zwp_text_input_manager_v3 = NULL;
    app->zwp_text_input_v3 = NULL;

    // Pointer internals.
    sb_pointer_priv_init(&app->pointer);
    app->pointer.sb_application = app;

    // Create output list.
    app->outputs = sb_list_new();

    app->text_input.wl_surface = NULL;
    app->text_input.preedit_text = NULL;
    app->text_input.commit_text = NULL;

    // Init xkb context as NULL.
    app->xkb_context = NULL;

    app->_wl_registry = wl_display_get_registry(app->wl_display);
    wl_registry_add_listener(app->_wl_registry, &app_registry_listener,
        (void*)app);

    wl_display_dispatch(app->wl_display);
    wl_display_roundtrip(app->wl_display);

    xdg_wm_base_add_listener(app->_xdg_wm_base, &app_xdg_wm_base_listener,
        NULL);

    // Desktop surface list.
    app->toplevels = sb_list_new();
    app->popups = sb_list_new();

    // Event dispatcher.
    app->event_dispatcher = sb_event_dispatcher_new();

    app->xcursor.manager = NULL;
    app->xcursor.current[0] = '\0';

    _sb_application_instance = app;

    app->egl = sb_egl_new();

    // Event listeners.
    app->event_listeners = sb_list_new();

    return app;
}

sb_application_t* sb_application_instance()
{
    return _sb_application_instance;
}

uint32_t sb_application_pointer_button_serial(sb_application_t *application)
{
    return application->pointer.button_serial;
}

void sb_application_load_cursor_themes(sb_application_t *application)
{
    if (application->xcursor.manager != NULL) {
        sb_xcursor_theme_manager_free(application->xcursor.manager);
    }
    // Load.
    application->xcursor.manager = sb_xcursor_theme_manager_load();
}

const sb_list_t* sb_application_cursor_theme_ids(sb_application_t *application)
{
    return application->xcursor.manager->ids;
}

void sb_application_set_cursor_theme(sb_application_t *application,
                                     const char *id)
{
    bool found = false;
    sb_list_t *ids = application->xcursor.manager->ids;
    for (uint64_t i = 0; i < sb_list_length(ids); ++i) {
        const char *it = sb_list_at(ids, i);
        if (strcmp(it, id) == 0) {
            found = true;
            break;
        }
    }
    if (!found) {
        sb_log_warn("No such cursor theme %s\n", id);
        return;
    }

    strcpy(application->xcursor.current, id);
}

const char* sb_application_cursor_theme(sb_application_t *application)
{
    if (strlen(application->xcursor.current) == 0) {
        return NULL;
    }
    return application->xcursor.current;
}

uint32_t sb_application_add_timer(sb_application_t *application,
                                  sb_surface_t *target,
                                  uint32_t interval,
                                  bool repeat)
{
    sb_event_t *event = sb_event_new(SB_EVENT_TARGET_TYPE_SURFACE,
        target, SB_EVENT_TYPE_TIMEOUT);

    event->timer.id = 0;
    event->timer.interval = interval;
    event->timer.repeat = repeat;
    event->timer.time = 0;

    // Add new timer event to the event dispatcher.
    uint32_t new_id = sb_event_dispatcher_timer_add_event(
        application->event_dispatcher, event);

    return new_id;
}

void sb_application_remove_timer(sb_application_t *application, uint32_t id)
{
    sb_event_dispatcher_timer_remove_event(application->event_dispatcher, id);
}

void sb_application_post_event(sb_application_t *application,
                               sb_event_t *event)
{
    sb_event_dispatcher_post_event(application->event_dispatcher, event);
}

void sb_application_register_desktop_surface(sb_application_t *application,
    sb_desktop_surface_t *desktop_surface)
{
    if (sb_desktop_surface_is_toplevel(desktop_surface)) {
        sb_list_push(application->toplevels, (void*)desktop_surface);
    } else if (sb_desktop_surface_is_popup(desktop_surface)) {
        sb_list_push(application->popups, (void*)desktop_surface);
    }
}

void sb_application_unregister_desktop_surface(sb_application_t *application,
    sb_desktop_surface_t *desktop_surface)
{
    if (sb_desktop_surface_is_toplevel(desktop_surface)) {
        sb_list_t *list = application->toplevels;
        uint64_t length = sb_list_length(list);
        int64_t index = -1;
        for (uint64_t i = 0; i < length; ++i) {
            if (sb_list_at(list, i) == desktop_surface) {
                index = i;
                break;
            }
        }
        if (index != -1) {
            sb_list_remove(list, index);
        }
    } else if (sb_desktop_surface_is_popup(desktop_surface)) {
        sb_list_t *list = application->popups;
        uint64_t length = sb_list_length(list);
        int64_t index = -1;
        for (uint64_t i = 0; i < length; ++i) {
            if (sb_list_at(list, i) == desktop_surface) {
                index = i;
                break;
            }
        }
        if (index != -1) {
            sb_list_remove(list, index);
        }
    }
}

struct wl_display* sb_application_wl_display(
    sb_application_t *application)
{
    return application->wl_display;
}

struct wl_compositor* sb_application_wl_compositor(
    sb_application_t *application)
{
    return application->_wl_compositor;
}

struct wl_shm* sb_application_wl_shm(sb_application_t *application)
{
    return application->wl_shm;
}

struct xdg_wm_base* sb_application_xdg_wm_base(sb_application_t *application)
{
    return application->_xdg_wm_base;
}

struct wl_seat* sb_application_wl_seat(sb_application_t *application)
{
    return application->_wl_seat;
}

struct zwp_text_input_v3* sb_application_zwp_text_input_v3(
    sb_application_t *application)
{
    return application->zwp_text_input_v3;
}

sb_egl_context_t* sb_application_egl_context(sb_application_t *application)
{
    return application->egl;
}

sb_egl_context_t* sb_application_egl(sb_application_t *application)
{
    return application->egl;
}

int sb_application_exec(sb_application_t *application)
{
    sb_event_dispatcher_t *dispatcher = application->event_dispatcher;

    struct pollfd poll_fds[] = {
        { wl_display_get_fd(application->wl_display), POLLIN },
        { sb_event_dispatcher_timer_fd(dispatcher), POLLIN },
        { sb_event_dispatcher_keyboard_key_repeat_fd(dispatcher), POLLIN },
    };

    int err = 0; // wl_display_dispatch(application->wl_display);
    while (err != -1) {
        int ret = poll(poll_fds, 3, -1);

        if (poll_fds[0].revents == POLLIN) {
            err = wl_display_dispatch(application->wl_display);
        }

        if (poll_fds[1].revents == POLLIN) {
            sb_event_dispatcher_timer_process_events(dispatcher);
        }

        if (poll_fds[2].revents == POLLIN) {
            sb_event_dispatcher_keyboard_key_repeat_process_events(dispatcher);
        }

        sb_event_dispatcher_process_events(dispatcher);
        // err = wl_display_dispatch(application->wl_display);

        sb_event_t *tick_event = sb_event_new(SB_EVENT_TARGET_TYPE_APPLICATION,
            application,
            SB_EVENT_TYPE_NEXT_TICK);
        sb_application_post_event(application, tick_event);

        // Exit event loop when last toplevel desktop surface closed.
        if (sb_list_length(application->toplevels) == 0) {
            sb_log_debug("Last toplevel desktop surface closed.\n");
            break;
        }

        wl_display_flush(application->wl_display);
    }

    if (err == -1) {
        sb_log_warn("Error on wl_display_dispatch().\n");
        return 1;
    }

    sb_log_debug("Destroying application resources ...\n");
    sb_egl_free(application->egl);

    sb_log_debug("Quit application.\n");

    return 0;
}


//!<==============
//!< Events
//!<==============

void sb_application_add_event_listener(sb_application_t *application,
                                       enum sb_event_type event_type,
                                       sb_event_listener_t listener,
                                       void *user_data)
{
    sb_event_listener_tuple_t *tuple = sb_event_listener_tuple_new(
        event_type, listener, user_data);
    sb_list_push(application->event_listeners, (void*)tuple);
}

void sb_application_on_next_tick(sb_application_t *application,
                                 sb_event_t *event)
{
    _event_listener_filter_for_each(application->event_listeners,
        SB_EVENT_TYPE_NEXT_TICK, event);
}


//!<==============
//!< Registry
//!<==============

static void app_global_handler(void *data,
                               struct wl_registry *wl_registry,
                               uint32_t name,
                               const char *interface,
                               uint32_t version)
{
    sb_application_t *app = (sb_application_t*)data;

    if (strcmp(interface, "wl_compositor") == 0) {
        app->_wl_compositor = wl_registry_bind(wl_registry,
            name, &wl_compositor_interface, version);
    } else if (strcmp(interface, "xdg_wm_base") == 0) {
        app->_xdg_wm_base = wl_registry_bind(wl_registry,
            name, &xdg_wm_base_interface, 1);
    } else if (strcmp(interface, "wl_seat") == 0) {
        app->_wl_seat = wl_registry_bind(wl_registry,
            name, &wl_seat_interface, 5);
        wl_seat_add_listener(app->_wl_seat, &seat_listener, (void*)app);
    } else if (strcmp(interface, "wl_output") == 0) {
        sb_log_debug("wl_output - name: %d\n", name);
        struct wl_output *wl_output = wl_registry_bind(wl_registry,
            name, &wl_output_interface, 4);
        // Add the output.
        {
            sb_output_t *output = sb_output_new(wl_output, name);
            sb_list_push(app->outputs, output);
        }
        wl_output_add_listener(wl_output, &output_listener, (void*)app);
    } else if (strcmp(interface, "wl_shm") == 0) {
        app->wl_shm = wl_registry_bind(wl_registry,
            name, &wl_shm_interface, 1);
        wl_shm_add_listener(app->wl_shm, &shm_listener, (void*)app);
    } else if (strcmp(interface, "wp_cursor_shape_manager_v1") == 0) {
        app->pointer.wp_cursor_shape_manager_v1 = wl_registry_bind(wl_registry,
            name, &wp_cursor_shape_manager_v1_interface, 1);
    } else if (strcmp(interface, "zwp_text_input_manager_v3") == 0) {
        app->zwp_text_input_manager_v3 = wl_registry_bind(wl_registry,
            name, &zwp_text_input_manager_v3_interface, 1);
    }
}

static void app_global_remove_handler(void *data,
                                      struct wl_registry *wl_registry,
                                      uint32_t name)
{
    sb_log_debug("global_remove_handler() - name: %d\n", name);
}

//!<===========
//!< XDG
//!<===========

static void xdg_wm_base_ping_handler(void *data,
                                     struct xdg_wm_base *xdg_wm_base,
                                     uint32_t serial)
{
    xdg_wm_base_pong(xdg_wm_base, serial);
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
    sb_application_t *application = (sb_application_t*)data;
    sb_output_t *output = _get_output(application, wl_output);
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
    sb_application_t *application = (sb_application_t*)data;
    sb_output_t *output = _get_output(application, wl_output);
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
    sb_application_t *application = (sb_application_t*)data;
    sb_output_t *output = _get_output(application, wl_output);
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

//!<============
//!< Keyboard
//!<============

static void keyboard_keymap_handler(void *data,
                                    struct wl_keyboard *wl_keyboard,
                                    uint32_t format,
                                    int32_t fd,
                                    uint32_t size)
{
    sb_application_t *application = (sb_application_t*)data;

    if (format == WL_KEYBOARD_KEYMAP_FORMAT_NO_KEYMAP) {
        sb_log_debug("Keyboard keymap NO_KEYMAP.\n");
    } else if (format == WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1) {
        sb_log_debug("Keyboard keymap XKB_V1\n");

        char *keymap_string = (char*)mmap(NULL, size,
            PROT_READ, MAP_PRIVATE, fd, 0);
        if (keymap_string == MAP_FAILED) {
            sb_log_error("Keymap map failed!\n");
            return;
        }

        application->xkb_context = sb_xkb_context_new(keymap_string);

        munmap(keymap_string, size);
    }
}

static void keyboard_enter_handler(void *data,
                                   struct wl_keyboard *wl_keyboard,
                                   uint32_t serial,
                                   struct wl_surface *wl_surface,
                                   struct wl_array *keys)
{
    sb_application_t *application = (sb_application_t*)data;

    sb_log_debug("keyboard_enter_handler - serial: %d\n", serial);

    sb_surface_t *surface = _find_surface(application, wl_surface);
    application->keyboard.surface = surface;
}

static void keyboard_leave_handler(void *data,
                                   struct wl_keyboard *wl_keyboard,
                                   uint32_t serial,
                                   struct wl_surface *wl_surface)
{
    sb_application_t *application = (sb_application_t*)data;
    (void)application;

    sb_log_debug("keyboard_leave_handler - serial: %d\n", serial);
}

static void keyboard_key_handler(void *data,
                                 struct wl_keyboard *wl_keyboard,
                                 uint32_t serial,
                                 uint32_t time,
                                 uint32_t key,
                                 uint32_t state)
{
    sb_application_t *application = (sb_application_t*)data;

    enum sb_event_type event_type = SB_EVENT_TYPE_KEYBOARD_KEY_PRESS;
    if (state == WL_KEYBOARD_KEY_STATE_PRESSED) {
        event_type = SB_EVENT_TYPE_KEYBOARD_KEY_PRESS;
    } else if (state == WL_KEYBOARD_KEY_STATE_RELEASED) {
        event_type = SB_EVENT_TYPE_KEYBOARD_KEY_RELEASE;
    } else {
        sb_log_warn("keyboard_key_handler - Unkown state.\n");
    }

    sb_event_t *event = sb_event_new(SB_EVENT_TARGET_TYPE_SURFACE,
        application->keyboard.surface,
        event_type);

    if (application->xkb_context != NULL) {
        uint32_t keysym = xkb_state_key_get_one_sym(
            application->xkb_context->xkb_state, key + 8);
        sb_log_debug("keyboard_key_handler - state: %d\n", state);
        sb_log_debug(" - Keysym: 0x%X\n", keysym);
        sb_log_debug(" - Keycode: %d\n", key);
        // sb_log_debug(" - serial: %d\n", serial);
        // sb_log_debug(" - event: %p\n", event);

        xkb_state_key_get_utf8(application->xkb_context->xkb_state, key + 8,
            event->keyboard.text, 16);

        sb_log_debug(" - text: %s\n", event->keyboard.text);

        event->keyboard.key = key;
        event->keyboard.keysym = keysym;
        event->keyboard.keycode = key;
        event->keyboard.repeated = false;
    } else {
        event->keyboard.key = 0;
        event->keyboard.keycode = key;
        event->keyboard.repeated = false;
    }

    sb_application_post_event(application, event);

    // Pass event to the event dispatcher to repeat.
    if (state == WL_KEYBOARD_KEY_STATE_PRESSED) {
        sb_event_dispatcher_keyboard_key_repeat_add_event(
            application->event_dispatcher, event);
    } else if (state == WL_KEYBOARD_KEY_STATE_RELEASED) {
        sb_event_dispatcher_keyboard_key_repeat_remove_event(
            application->event_dispatcher, event);
    }
}

static void keyboard_modifiers_handler(void *data,
                                       struct wl_keyboard *wl_keyboard,
                                       uint32_t serial,
                                       uint32_t mods_depressed,
                                       uint32_t mods_latched,
                                       uint32_t mods_locked,
                                       uint32_t group)
{
    sb_application_t *application = (sb_application_t*)data;

    sb_log_debug("keyboard_modifiers_handler\n");
    sb_log_debug(" - mods_depressed: %d\n", mods_depressed);
    sb_log_debug(" - mods_latched: %d\n", mods_latched);
    sb_log_debug(" - mods_locked: %d\n", mods_locked);
    sb_log_debug(" - group: %d\n", group);

    if (application->xkb_context != NULL) {
        xkb_state_update_mask(application->xkb_context->xkb_state,
            mods_depressed,
            mods_latched,
            mods_locked,
            group, group, group
        );
    }
}

static void keyboard_repeat_info_handler(void *data,
                                         struct wl_keyboard *wl_keyboard,
                                         int32_t rate,
                                         int32_t delay)
{
    sb_application_t *application = (sb_application_t*)data;

    sb_log_debug("Keyboard repeat - rate: %d, delay: %d\n", rate, delay);

    sb_event_dispatcher_keyboard_key_repeat_set_delay(
        application->event_dispatcher, delay);
    sb_event_dispatcher_keyboard_key_repeat_set_rate(
        application->event_dispatcher, rate);
}

//!<=========
//!< Seat
//!<=========

static void seat_capabilities_handler(void *data,
                                      struct wl_seat *wl_seat,
                                      uint32_t capabilities)
{
    sb_application_t *app = (sb_application_t*)data;

    if (capabilities & WL_SEAT_CAPABILITY_POINTER) {
        app->pointer.wl_pointer = wl_seat_get_pointer(wl_seat);
        sb_pointer_priv_add_listener(&app->pointer, wl_seat);
    }
    if (capabilities & WL_SEAT_CAPABILITY_KEYBOARD) {
        app->_wl_keyboard = wl_seat_get_keyboard(wl_seat);
        wl_keyboard_add_listener(app->_wl_keyboard, &keyboard_listener,
            (void*)app);

        if (app->zwp_text_input_manager_v3 != NULL) {
            app->zwp_text_input_v3 = zwp_text_input_manager_v3_get_text_input(
                app->zwp_text_input_manager_v3, wl_seat);
            zwp_text_input_v3_add_listener(app->zwp_text_input_v3,
                &text_input_listener, (void*)app);
        }
    }
}

static void seat_name_handler(void *data,
                              struct wl_seat *wl_seat,
                              const char *name)
{
    sb_log_debug("Seat name: %s\n", name);
}

//!<====================
//!< Shm
//!<====================

static void shm_format_handler(void *data, struct wl_shm *wl_shm, uint32_t format)
{
    sb_log_debug("Shm format: %d\n", format);
}

static void buffer_release_handler(void *data, struct wl_buffer *buffer)
{
    // TODO.
}

//!<====================
//!< Text Input
//!<====================

static void text_input_enter_handler(void *data,
                                     struct zwp_text_input_v3 *text_input,
                                     struct wl_surface *wl_surface)
{
    sb_application_t *app = (sb_application_t*)data;

    app->text_input.wl_surface = wl_surface;
}

static void text_input_leave_handler(void *data,
                                     struct zwp_text_input_v3 *text_input,
                                     struct wl_surface *wl_surface)
{
    sb_application_t *app = (sb_application_t*)data;

    app->text_input.wl_surface = NULL;
}

static void text_input_preedit_string_handler(void *data,
    struct zwp_text_input_v3 *text_input,
    const char *text,
    int32_t cursor_begin,
    int32_t cursor_end)
{
    sb_application_t *app = (sb_application_t*)data;
    sb_surface_t *surface = _find_surface(app, app->text_input.wl_surface);

    if (surface != NULL) {
        sb_view_t *view = sb_surface_focused_view(surface);
        sb_log_debug("text_input_preedit_string_handler - %s\n", text);
        sb_log_debug(" |- cursor begin/end: %d %d\n", cursor_begin, cursor_end);
        app->text_input.preedit_text = text;

        if (view != NULL) {
            sb_event_t *event = sb_event_new(SB_EVENT_TARGET_TYPE_VIEW, view,
                SB_EVENT_TYPE_TEXT_INPUT);
            event->text_input.preedit_string = text;
            event->text_input.commit_string = NULL;
            event->text_input.index = cursor_begin;

            sb_application_post_event(app, event);
        }
    }
}

static void text_input_commit_string_handler(void *data,
    struct zwp_text_input_v3 *text_input,
    const char *text)
{
    sb_application_t *app = (sb_application_t*)data;
    sb_surface_t *surface = _find_surface(app, app->text_input.wl_surface);

    if (surface != NULL) {
        sb_view_t *view = sb_surface_focused_view(surface);
        sb_log_debug("text_input_commit_string_handler - %s\n", text);
        app->text_input.commit_text = text;

        if (view != NULL) {
            sb_event_t *event = sb_event_new(SB_EVENT_TARGET_TYPE_VIEW, view,
                SB_EVENT_TYPE_TEXT_INPUT);
            event->text_input.preedit_string = NULL;
            event->text_input.commit_string = text;
            event->text_input.index = 0;

            sb_application_post_event(app, event);
        }
    }
}

static void text_input_delete_surrounding_text_handler(void *data,
    struct zwp_text_input_v3 *zwp_text_input_v3,
    uint32_t before_length,
    uint32_t after_length)
{
    sb_log_debug("text_input_delete_surrounding_text_handler\n");
}

static void text_input_done_handler(void *data,
                                    struct zwp_text_input_v3 *text_input,
                                    uint32_t serial)
{
    // sb_application_t *app = (sb_application_t*)data;

    sb_log_debug("text_input_done_handler\n");
    // sb_log_debug(" |- preedit: %s\n", app->text_input.preedit_text);
    // sb_log_debug(" |- commit:  %s\n", app->text_input.commit_text);
}


sb_surface_t* sb_application_find_surface_by_wl_surface(
    sb_application_t *application, struct wl_surface *wl_surface)
{
    return _find_surface(application, wl_surface);
}
