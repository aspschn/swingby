#include <swingby/application.h>

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <unistd.h>
#include <sys/mman.h>

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

#include "xkb/xkb-context.h"
#include "xcursor/xcursor.h"

#include "../../helpers/shared.h"
#include "helpers/application.h"

struct sb_application_t {
    /// `struct wl_display`.
    struct wl_display *_wl_display;
    /// `struct wl_registry`.
    struct wl_registry *_wl_registry;
    struct wl_compositor *_wl_compositor;
    struct xdg_wm_base *_xdg_wm_base;
    struct wl_seat *_wl_seat;
    struct wl_pointer *_wl_pointer;
    struct wl_keyboard *_wl_keyboard;
    struct wl_touch *_wl_touch;
    struct wp_cursor_shape_manager_v1 *wp_cursor_shape_manager_v1;
    struct zwp_text_input_manager_v3 *zwp_text_input_manager_v3;
    struct zwp_text_input_v3 *zwp_text_input_v3;
    struct {
        /// \brief Current pointer surface.
        ///
        /// Pointer motion handler not pass `struct wl_surface` object.
        /// Store this information when pointer entered to the surface.
        struct wl_surface *wl_surface;
        /// \brief Current pointer view.
        ///
        /// Store the position of the view under the pointer.
        /// Usefull when check differences in motion event.
        sb_view_t *view;
        /// \brief Pointer event position.
        ///
        /// Pointer button and axis event not pass the position.
        /// Store this information when pointer moved.
        sb_point_t pos;
        /// \brief Pointer button event serial.
        uint32_t button_serial;
        /// \brief Pointer enter event information.
        ///
        /// Currently used only when change cursor shape.
        uint32_t enter_serial;
    } pointer;
    /// \brief Click event information.
    struct {
        sb_view_t *view;
        sb_pointer_button button;
    } click;
    struct {
        sb_view_t *view;
        uint32_t click_count;
        uint32_t time;
        sb_pointer_button button;
    } double_click;
    struct {
        enum sb_pointer_scroll_axis axis;
        enum sb_pointer_scroll_source source;
        float value;
        /// \brief Vertical stop.
        bool ver_stop;
        /// \brief Horizontal stop.
        bool hor_stop;
        enum sb_pointer_scroll_axis stop_axis;
        bool frame;
    } scroll;
    struct {
        /// \brief Current keyboard surface.
        sb_surface_t *surface;
    } keyboard;
    struct {
        struct wl_surface *wl_surface;
    } text_input;
    struct sb_xkb_context_t *xkb_context;
    /// \brief List of the desktop surfaces.
    sb_list_t *desktop_surfaces;
    struct {
        sb_xcursor_theme_manager_t *manager;
        char current[256];
    } xcursor;
    /// \brief Default cursor when view not set cursor.
    sb_cursor_t *cursor;
    /// \brief Output list.
    sb_list_t *outputs;
    /// \brief An event dispatcher.
    sb_event_dispatcher_t *event_dispatcher;
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

//!<===========
//!< Pointer
//!<===========

static void pointer_enter_handler(void *data,
                                  struct wl_pointer *wl_pointer,
                                  uint32_t serial,
                                  struct wl_surface *surface,
                                  wl_fixed_t sx,
                                  wl_fixed_t sy);

static void pointer_leave_handler(void *data,
                                  struct wl_pointer *pointer,
                                  uint32_t serial,
                                  struct wl_surface *surface);

static void pointer_motion_handler(void *data,
                                   struct wl_pointer *wl_pointer,
                                   uint32_t time,
                                   wl_fixed_t sx,
                                   wl_fixed_t sy);

static void pointer_button_handler(void *data,
                                   struct wl_pointer *pointer,
                                   uint32_t serial,
                                   uint32_t time,
                                   uint32_t button,
                                   uint32_t state);

static void pointer_axis_handler(void *data,
                                 struct wl_pointer *wl_pointer,
                                 uint32_t time,
                                 uint32_t axis,
                                 wl_fixed_t value);

static void pointer_frame_handler(void *data,
                                  struct wl_pointer *wl_pointer);

static void pointer_axis_source_handler(void *data,
                                        struct wl_pointer *wl_pointer,
                                        uint32_t axis_source);

static void pointer_axis_stop_handler(void *data,
                                      struct wl_pointer *wl_pointer,
                                      uint32_t time,
                                      uint32_t axis);

static void pointer_axis_discrete_handler(void *data,
                                          struct wl_pointer *wl_pointer,
                                          uint32_t axis,
                                          int32_t discrete);

static const struct wl_pointer_listener pointer_listener = {
    .enter = pointer_enter_handler,
    .leave = pointer_leave_handler,
    .motion = pointer_motion_handler,
    .button = pointer_button_handler,
    .axis = pointer_axis_handler,
    .frame = pointer_frame_handler,
    .axis_source = pointer_axis_source_handler,
    .axis_stop = pointer_axis_stop_handler,
    .axis_discrete = pointer_axis_discrete_handler, // Deprecated since 8.
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
    sb_list_t *list = app->desktop_surfaces;
    for (int i = 0; i < sb_list_length(list); ++i) {
        sb_desktop_surface_t *desktop_surface = sb_list_at(list, i);
        sb_surface_t *surface = sb_desktop_surface_surface(desktop_surface);
        if (sb_surface_wl_surface(surface) == wl_surface) {
            found = surface;
            break;
        }
    }

    return found;
}

/// \brief Linux button to Foundation pointer button.
sb_pointer_button _from_linux_button(uint32_t button)
{
    switch (button) {
    case BTN_LEFT:
        return SB_POINTER_BUTTON_LEFT;
    case BTN_RIGHT:
        return SB_POINTER_BUTTON_RIGHT;
    case BTN_MIDDLE:
        return SB_POINTER_BUTTON_MIDDLE;
    default:
        return SB_POINTER_BUTTON_UNIMPLEMENTED;
    }
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

static void _reset_double_click(sb_application_t *application)
{
    application->double_click.view = NULL;
    application->double_click.click_count = 0;
    application->double_click.time = 0;
    application->double_click.button = SB_POINTER_BUTTON_NONE;
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

static void _change_cursor_shape(sb_application_t *application,
                                 struct wl_pointer *wl_pointer,
                                 enum sb_cursor_shape shape)
{
    if (application->wp_cursor_shape_manager_v1 != NULL) {
        struct wp_cursor_shape_device_v1 *device =
            wp_cursor_shape_manager_v1_get_pointer(
                application->wp_cursor_shape_manager_v1,
                wl_pointer
            );
        wp_cursor_shape_device_v1_set_shape(device,
            application->pointer.enter_serial,
            _to_wp_cursor_shape(shape));
        wp_cursor_shape_device_v1_destroy(device);
    }
}

//!<===============
//!< Application
//!<===============

sb_application_t* sb_application_new(int argc, char *argv[])
{
    sb_application_t *app = malloc(sizeof(sb_application_t));

    app->_wl_display = wl_display_connect(NULL);

    // Null initializations.
    app->_wl_seat = NULL;
    app->_wl_pointer = NULL;
    app->_wl_keyboard = NULL;
    app->_wl_touch = NULL;
    app->wp_cursor_shape_manager_v1 = NULL;
    app->zwp_text_input_manager_v3 = NULL;
    app->zwp_text_input_v3 = NULL;

    app->pointer.wl_surface = NULL;
    app->pointer.view = NULL;
    app->pointer.pos.x = 0;
    app->pointer.pos.y = 0;

    app->click.view = NULL;
    app->click.button = SB_POINTER_BUTTON_NONE;

    // Create output list.
    app->outputs = sb_list_new();

    _reset_double_click(app);

    // Init scroll info.
    app->scroll.value = 0.0f;
    app->scroll.ver_stop = false;
    app->scroll.hor_stop = false;
    app->scroll.frame = false;

    app->text_input.wl_surface = NULL;

    // Init xkb context as NULL.
    app->xkb_context = NULL;

    app->_wl_registry = wl_display_get_registry(app->_wl_display);
    wl_registry_add_listener(app->_wl_registry, &app_registry_listener,
        (void*)app);

    wl_display_dispatch(app->_wl_display);
    wl_display_roundtrip(app->_wl_display);

    xdg_wm_base_add_listener(app->_xdg_wm_base, &app_xdg_wm_base_listener,
        NULL);

    // Desktop surface list.
    app->desktop_surfaces = sb_list_new();

    // Event dispatcher.
    app->event_dispatcher = sb_event_dispatcher_new();

    app->xcursor.manager = NULL;
    app->xcursor.current[0] = '\0';
    app->cursor = NULL;

    _sb_application_instance = app;

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
    sb_list_push(application->desktop_surfaces, (void*)desktop_surface);
}

void sb_application_unregister_desktop_surface(sb_application_t *application,
    sb_desktop_surface_t *desktop_surface)
{
    // TODO: Implementation.
    sb_list_t *list = application->desktop_surfaces;
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

struct wl_display* sb_application_wl_display(
    sb_application_t *application)
{
    return application->_wl_display;
}

struct wl_compositor* sb_application_wl_compositor(
    sb_application_t *application)
{
    return application->_wl_compositor;
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

int sb_application_exec(sb_application_t *application)
{
    int err = wl_display_dispatch(application->_wl_display);
    while (err != -1) {
        sb_event_dispatcher_process_events(application->event_dispatcher);

        // Exit event loop when last desktop surface closed.
        if (sb_list_length(application->desktop_surfaces) == 0) {
            sb_log_debug("Last desktop surface closed.\n");
            break;
        }

        wl_display_flush(application->_wl_display);

        // Keyboard key repeat.
        bool has_event = sb_event_dispatcher_keyboard_key_repeat_has_event(
            application->event_dispatcher);
        has_event = has_event || sb_event_dispatcher_timer_has_event(
            application->event_dispatcher);
        if (has_event != true) {
            err = wl_display_dispatch(application->_wl_display);
        } else {
            err = wl_display_dispatch_pending(application->_wl_display);
            // Throttle to prevent 100% CPU usage.
            usleep(500);
            int prepare = wl_display_prepare_read(application->_wl_display);
            if (prepare == 0) {
                wl_display_read_events(application->_wl_display);
                err = wl_display_dispatch_pending(application->_wl_display);
            }
        }
    }

    if (err == -1) {
        sb_log_warn("Error on wl_display_dispatch().\n");
        return 1;
    }

    sb_log_debug("Quit application.\n");

    return 0;
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
    } else if (strcmp(interface, "wp_cursor_shape_manager_v1") == 0) {
        app->wp_cursor_shape_manager_v1 = wl_registry_bind(wl_registry,
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
//!< Pointer
//!<============

static void pointer_enter_handler(void *data,
                                  struct wl_pointer *wl_pointer,
                                  uint32_t serial,
                                  struct wl_surface *wl_surface,
                                  wl_fixed_t sx,
                                  wl_fixed_t sy)
{
    sb_application_t *app = (sb_application_t*)data;

    app->pointer.wl_surface = wl_surface;

    // Set the serial.
    app->pointer.enter_serial = serial;

    // Cursor shape.
    if (app->wp_cursor_shape_manager_v1 != NULL) {
        struct wp_cursor_shape_device_v1 *device =
            wp_cursor_shape_manager_v1_get_pointer(
                app->wp_cursor_shape_manager_v1,
                wl_pointer
            );
        wp_cursor_shape_device_v1_set_shape(device, serial,
            WP_CURSOR_SHAPE_DEVICE_V1_SHAPE_DEFAULT);

        wp_cursor_shape_device_v1_destroy(device);
    } else {
        // TEST cursor.
        // Set default cursor.
        if (app->cursor == NULL) {
            sb_point_t hot_spot;
            hot_spot.x = 0;
            hot_spot.y = 0;
            app->cursor = sb_cursor_new(SB_CURSOR_SHAPE_DEFAULT, &hot_spot);
        }

        sb_surface_t *cursor_surface = sb_cursor_surface(app->cursor);
        wl_pointer_set_cursor(wl_pointer,
            serial, sb_surface_wl_surface(cursor_surface), 0, 0);
    }

    float x = wl_fixed_to_double(sx);
    float y = wl_fixed_to_double(sy);

    // Store the pointer position.
    // Since while resizing the desktop surface, there is no motion event.
    // Instead the surface enter event is fired. So this is important to
    // re-assign pointer position when enter event.
    //
    // Remember: The pointer enter event could be fired even mouse pointer is
    // already in the surface area.
    app->pointer.pos.x = x;
    app->pointer.pos.y = y;

    // Find the surface.
    sb_surface_t *found = _find_surface(app, wl_surface);

    // Make an event.
    sb_event_t *event = sb_event_new(SB_EVENT_TARGET_TYPE_SURFACE,
        (void*)found,
        SB_EVENT_TYPE_POINTER_ENTER);
    event->pointer.button = SB_POINTER_BUTTON_NONE;
    event->pointer.position.x = x;
    event->pointer.position.y = y;

    // Post the event.
    sb_application_post_event(app, event);

    // Find most child.
    sb_view_t *root_view = sb_surface_root_view(found);
    sb_point_t position;
    position.x = x;
    position.y = y;
    sb_log_debug(" == root view: %p ==\n", root_view);
    sb_view_t *view = _find_most_child(root_view, &position);

    app->pointer.view = view;

    // Change cursor shape.
    _change_cursor_shape(app, wl_pointer, sb_view_cursor_shape(view));

    // Post the event (view).
    _post_pointer_enter_event(view, position.x, position.y);
}

static void pointer_leave_handler(void *data,
                                  struct wl_pointer *pointer,
                                  uint32_t serial,
                                  struct wl_surface *surface)
{
    //
}

static void pointer_motion_handler(void *data,
                                   struct wl_pointer *wl_pointer,
                                   uint32_t time,
                                   wl_fixed_t sx,
                                   wl_fixed_t sy)
{
    sb_application_t *app = (sb_application_t*)data;

    float x = wl_fixed_to_double(sx);
    float y = wl_fixed_to_double(sy);

    // Store the position.
    app->pointer.pos.x = x;
    app->pointer.pos.y = y;

    // Find the surface.
    sb_surface_t *surface = _find_surface(app, app->pointer.wl_surface);

    // Find most child view.
    sb_point_t pos;
    pos.x = x;
    pos.y = y;
    sb_view_t *view = _find_most_child(sb_surface_root_view(surface), &pos);

    // Pointer move event.
    {
        sb_event_t *move_event = sb_event_new(SB_EVENT_TARGET_TYPE_VIEW,
            (void*)view,
            SB_EVENT_TYPE_POINTER_MOVE);
        move_event->pointer.button = SB_POINTER_BUTTON_NONE;
        move_event->pointer.position.x = pos.x;
        move_event->pointer.position.y = pos.y;

        sb_application_post_event(app, move_event);
    }

    // Check difference.
    if (view != app->pointer.view) {
        // Post the leave event for the previous view.
        // TODO: Leave position.
        _post_pointer_leave_event(app->pointer.view, 0.0f, 0.0f);

        app->pointer.view = view;

        // Cursor shape.
        _change_cursor_shape(app, wl_pointer, sb_view_cursor_shape(view));

        // Post the event.
        _post_pointer_enter_event(view, pos.x, pos.y);
    }
}

static void pointer_button_handler(void *data,
                                   struct wl_pointer *pointer,
                                   uint32_t serial,
                                   uint32_t time,
                                   uint32_t button,
                                   uint32_t state)
{
    sb_application_t *app = (sb_application_t*)data;

    app->pointer.button_serial = serial;

    float x = app->pointer.pos.x;
    float y = app->pointer.pos.y;

    // Find the surface.
    sb_surface_t *surface = _find_surface(app, app->pointer.wl_surface);

    // Find most child view.
    sb_point_t pos = { .x = x, .y = y };
    sb_view_t *view = _find_most_child(sb_surface_root_view(surface), &pos);

    // Set the event type.
    enum sb_event_type evt_type = SB_EVENT_TYPE_POINTER_PRESS;
    if (state == WL_POINTER_BUTTON_STATE_PRESSED) {
        evt_type = SB_EVENT_TYPE_POINTER_PRESS;
    } else if (state == WL_POINTER_BUTTON_STATE_RELEASED) {
        evt_type = SB_EVENT_TYPE_POINTER_RELEASE;
    }
    sb_event_t *event = sb_event_new(SB_EVENT_TARGET_TYPE_VIEW,
        (void*)view,
        evt_type);

    event->pointer.button = _from_linux_button(button);
    event->pointer.position = pos;

    // Post the event.
    sb_application_post_event(app, event);

    // Click event.
    if (state == WL_POINTER_BUTTON_STATE_PRESSED) {
        app->click.view = view;
        app->click.button = _from_linux_button(button);
    } else if (state == WL_POINTER_BUTTON_STATE_RELEASED) {
        if (view == app->click.view) {
            sb_event_t *click_event = sb_event_new(SB_EVENT_TARGET_TYPE_VIEW,
                (void*)view,
                SB_EVENT_TYPE_POINTER_CLICK);
            click_event->pointer.button = app->click.button;
            click_event->pointer.position = pos;

            app->click.view = NULL;

            sb_application_post_event(app, click_event);

            // Double click.
            app->double_click.click_count += 1;

            if (app->double_click.view == NULL) {
                app->double_click.view = view;
                app->double_click.button = _from_linux_button(button);
            }
            // Reset double click info if different view or button.
            if (app->double_click.view != view &&
                app->double_click.button != _from_linux_button(button)) {
                _reset_double_click(app);
            }
            // Store time if click count is 1.
            if (app->double_click.click_count == 1) {
                app->double_click.time = time;
            }
            if (app->double_click.click_count == 2 &&
                app->double_click.view == view) {
                uint32_t diff = time - app->double_click.time;
                if (diff <= 1000) {
                    sb_log_debug("DOUBLE CLICK! %p\n", view);
                    sb_event_t *dbl_click_event = sb_event_new(
                        SB_EVENT_TARGET_TYPE_VIEW,
                        view,
                        SB_EVENT_TYPE_POINTER_DOUBLE_CLICK);
                    sb_application_post_event(app, dbl_click_event);
                }
                _reset_double_click(app);
            }
            if (app->double_click.click_count > 2) {
                _reset_double_click(app);
            }
        }
    }
}

static void pointer_axis_handler(void *data,
                                 struct wl_pointer *wl_pointer,
                                 uint32_t time,
                                 uint32_t axis,
                                 wl_fixed_t value)
{
    sb_application_t *app = (sb_application_t*)data;

    float val = wl_fixed_to_double(value);
    sb_log_debug("pointer_axis_handler() - value: %.2f, axis: %d\n", val, axis);

    enum sb_pointer_scroll_axis sb_axis =
        SB_POINTER_SCROLL_AXIS_VERTICAL_SCROLL;
    switch (axis) {
    case WL_POINTER_AXIS_VERTICAL_SCROLL:
        sb_axis = SB_POINTER_SCROLL_AXIS_VERTICAL_SCROLL;
        break;
    case WL_POINTER_AXIS_HORIZONTAL_SCROLL:
        sb_axis = SB_POINTER_SCROLL_AXIS_HORIZONTAL_SCROLL;
        break;
    default:
        break;
    }

    app->scroll.frame = false;
    app->scroll.axis = sb_axis;
    app->scroll.value = val;
}

static void pointer_frame_handler(void *data,
                                  struct wl_pointer *wl_pointer)
{
    sb_application_t *app = (sb_application_t*)data;

    // sb_log_debug("pointer_frame_handler()\n");
    if (app->scroll.frame == false && app->scroll.value != 0.0f) {
        sb_log_debug(" = Pointer frame for scroll.\n");
        // Post scroll event.
        sb_event_t *event = sb_event_new(SB_EVENT_TARGET_TYPE_VIEW,
            app->pointer.view, SB_EVENT_TYPE_POINTER_SCROLL);
        event->scroll.axis = app->scroll.axis;
        event->scroll.source = app->scroll.source;
        event->scroll.value = app->scroll.value;

        sb_application_post_event(app, event);

        // Reset scroll info.
        app->scroll.frame = true;
        app->scroll.value = 0.0f;
    }
    if (app->scroll.ver_stop == true || app->scroll.hor_stop == true) {
        sb_log_debug(" == Pointer frame for stop!\n");
        app->scroll.ver_stop = false;
        app->scroll.hor_stop = false;
    }
    // sb_log_debug("pointer_frame_handler()\n");
}

static void pointer_axis_source_handler(void *data,
                                        struct wl_pointer *wl_pointer,
                                        uint32_t axis_source)
{
    sb_application_t *app = (sb_application_t*)data;

    enum sb_pointer_scroll_source source;
    switch (axis_source) {
    case WL_POINTER_AXIS_SOURCE_WHEEL:
        source = SB_POINTER_SCROLL_SOURCE_WHEEL;
        sb_log_debug("pointer_axis_source_handler() - SB_POINTER_SCROLL_SOURCE_WHEEL\n");
        break;
    case WL_POINTER_AXIS_SOURCE_FINGER:
        source = SB_POINTER_SCROLL_SOURCE_FINGER;
        sb_log_debug("pointer_axis_source_handler() - SB_POINTER_SCROLL_SOURCE_FINGER\n");
        break;
    case WL_POINTER_AXIS_SOURCE_CONTINUOUS:
        source = SB_POINTER_SCROLL_SOURCE_CONTINUOUS;
        sb_log_debug("pointer_axis_source_handler() - SB_POINTER_SCROLL_SOURCE_CONTINUOUS\n");
        break;
    case WL_POINTER_AXIS_SOURCE_WHEEL_TILT:
        source = SB_POINTER_SCROLL_SOURCE_WHEEL_TILT;
        sb_log_debug("pointer_axis_source_handler() - SB_POINTER_SCROLL_SOURCE_WHEEL_TILT\n");
        break;
    default:
        source = SB_POINTER_SCROLL_SOURCE_WHEEL;
        break;
    }

    app->scroll.frame = false;
    app->scroll.source = source;
}

static void pointer_axis_stop_handler(void *data,
                                      struct wl_pointer *wl_pointer,
                                      uint32_t time,
                                      uint32_t axis)
{
    sb_application_t *app = (sb_application_t*)data;

    enum sb_pointer_scroll_axis sb_axis =
        SB_POINTER_SCROLL_AXIS_VERTICAL_SCROLL;
    switch (axis) {
    case WL_POINTER_AXIS_VERTICAL_SCROLL:
        app->scroll.ver_stop = true;
        sb_axis = SB_POINTER_SCROLL_AXIS_VERTICAL_SCROLL;
        break;
    case WL_POINTER_AXIS_HORIZONTAL_SCROLL:
        app->scroll.hor_stop = true;
        sb_axis = SB_POINTER_SCROLL_AXIS_HORIZONTAL_SCROLL;
        break;
    default:
        break;
    }

    app->scroll.stop_axis = sb_axis;
    app->scroll.frame = false;
    sb_log_debug("pointer_axis_stop_handler() - axis: %d\n", axis);

}

static void pointer_axis_discrete_handler(void *data,
                                          struct wl_pointer *wl_pointer,
                                          uint32_t axis,
                                          int32_t discrete)
{
    // Deprecated.
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
        sb_log_debug(" - serial: %d\n", serial);
        sb_log_debug(" - event: %p\n", event);

        xkb_state_key_get_utf8(application->xkb_context->xkb_state, key + 8,
            event->keyboard.text, 16);

        sb_log_debug(" - text: %s\n", event->keyboard.text);

        event->keyboard.key = keysym;
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
        app->_wl_pointer = wl_seat_get_pointer(wl_seat);
        wl_pointer_add_listener(app->_wl_pointer, &pointer_listener,
            (void*)app);
    }
    if (capabilities & WL_SEAT_CAPABILITY_KEYBOARD) {
        app->_wl_keyboard = wl_seat_get_keyboard(wl_seat);
        wl_keyboard_add_listener(app->_wl_keyboard, &keyboard_listener,
            (void*)app);

        app->zwp_text_input_v3 = zwp_text_input_manager_v3_get_text_input(
            app->zwp_text_input_manager_v3, wl_seat);
        zwp_text_input_v3_add_listener(app->zwp_text_input_v3,
            &text_input_listener, (void*)app);
    }
}

static void seat_name_handler(void *data,
                              struct wl_seat *wl_seat,
                              const char *name)
{
    sb_log_debug("Seat name: %s\n", name);
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
        (void)view;
        sb_log_debug("text_input_preedit_string_handler - %s\n", text);
    }
}

static void text_input_commit_string_handler(void *data,
    struct zwp_text_input_v3 *text_input,
    const char *text)
{
    sb_application_t *app = (sb_application_t*)data;
    sb_surface_t *surface = _find_surface(app, app->text_input.wl_surface);

    if (surface != NULL) {
        sb_log_debug("text_input_commit_string_handler - %s\n", text);
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
    sb_log_debug("text_input_done_handler\n");
}
