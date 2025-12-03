#include <swingby/surface.h>

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <sys/time.h>

#include <wayland-client.h>
#include <wayland-egl.h>
#include <wayland-protocols/unstable/text-input-unstable-v3.h>

#include <EGL/egl.h>
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>

#include <swingby/log.h>
#include <swingby/bench.h>
#include <swingby/application.h>
#include <swingby/view.h>
#include <swingby/image.h>
#include <swingby/list.h>
#include <swingby/event.h>

#include "../../helpers/shared.h"

#include "egl-context/egl-context.h"

// #include "../../skia/context.h"
#include "../../skia/renderer.h"
#include "../../skia/gl-renderer.h"
#include "../../skia/draw.h"

#include "../../shaders.h"

#define SWINGBY_BACKEND_DEFAULT "raster"

struct sb_surface_t {
    struct wl_surface *_wl_surface;
    struct wl_egl_window *_wl_egl_window;
    EGLSurface _egl_surface;
    // sb_egl_context_t *egl_context;
    sb_skia_renderer_t *skia_renderer;
    sb_size_t _size;
    sb_view_t *_root_view;
    uint32_t scale;
    sb_view_t *focused_view;
    bool frame_ready;
    bool update_pending;
    /// \brief Program objects for OpenGL.
    struct {
        GLuint color;
        GLuint texture;
    } programs;
    struct {
        GLuint vert_shader;
        GLuint frag_shader;
        GLuint program;
    } gl;
    struct wl_callback *frame_callback;
    sb_list_t *event_listeners;
};

//!<===============
//!< Callback
//!<===============

static void callback_done_handler(void *data,
                                  struct wl_callback *wl_callback,
                                  uint32_t time);

static const struct wl_callback_listener callback_listener = {
    .done = callback_done_handler,
};

//!<===================
//!< Wayland Surface
//!<===================

static void enter_handler(void *data,
                          struct wl_surface *wl_surface,
                          struct wl_output *wl_output);

static void leave_handler(void *data,
                          struct wl_surface *wl_surface,
                          struct wl_output *wl_output);

static void preferred_buffer_scale_handler(void *data,
                                           struct wl_surface *wl_surface,
                                           int32_t factor);

static void preferred_buffer_transform_handler(void *data,
                                               struct wl_surface *wl_surface,
                                               uint32_t transform);

static const struct wl_surface_listener surface_listener = {
    .enter = enter_handler,
    .leave = leave_handler,
    .preferred_buffer_scale = preferred_buffer_scale_handler,
    .preferred_buffer_transform = preferred_buffer_transform_handler,
};

//!<======================
//!< Helper Functions
//!<======================

void _gl_init(sb_surface_t *surface)
{
    /*
    eglMakeCurrent(surface->_egl_context->egl_display,
        surface->_egl_surface,
        surface->_egl_surface,
        surface->_egl_context->egl_context);

    glViewport(0, 0,
        surface->_size.width * surface->scale,
        surface->_size.height * surface->scale);

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,
        GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    // glUseProgram();

    // eglSwapBuffers(surface->_egl_context->egl_display, surface->_egl_surface);
    */
}

/// \brief Compile shader source and returns the shader object.
static GLuint _load_shader(const char *shader_source, GLuint type)
{
    GLuint shader;
    GLint compiled;

    shader = glCreateShader(type);
    if (shader == 0) {
        return 0;
    }

    glShaderSource(shader, 1, &shader_source, NULL);

    glCompileShader(shader);

    // Check the compile status.
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        GLint info_len = 0;

        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_len);
        if (info_len > 1) {
            char *info_log = malloc(sizeof(char));
            // fprintf(stderr, "Error compiling shader: %s\n", info_log);
            free(info_log);
        }

        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

/// \brief Calculate points used in the vertex shader based on the rect.
///
/// The vertex shader requires four points (top-left, bottom-left,
/// bottom-right, top-right).
/// This function calculate that points in a display coordinate. Then the
/// vertex shader converts each point to the OpenGL coordinate (from -1.0 to
/// 1.0).
static void _calc_points(const sb_rect_t *rect, float *points)
{
    float x = rect->pos.x;
    float y = rect->pos.y;
    float width = rect->size.width;
    float height = rect->size.height;

    // Top-left.
    points[0] = x;
    points[1] = y;
    // Bottom-left.
    points[3] = x;
    points[4] = y + height;
    // Bottom-right.
    points[6] = x + width;
    points[7] = y + height;
    // Top-right.
    points[9] = x + width;
    points[10] = y;
}

/// \brief Set the uniform variable `resolution`.
///
/// Uniform `resolution` used in the vertex shader to calculate OpenGL
/// coordinate.
///
/// The `resolution` should be same as surface's width and height.
static void _set_uniform_resolution(GLuint program, const sb_size_t *resolution)
{
    GLuint location = glGetUniformLocation(program, "resolution");
    float resolution_u[2] = { resolution->width, resolution->height };
    glUniform2fv(location, 1, resolution_u);
}

/// \brief Set the uniform variable `color`.
static void _set_uniform_color(GLuint program, const sb_color_t *color)
{
    GLuint location = glGetUniformLocation(program, "color");
    float color_u[4] = {
        color->r / 255.0f,
        color->g / 255.0f,
        color->b / 255.0f,
        color->a / 255.0f,
    };
    glUniform4fv(location, 1, color_u);
}

static void _set_uniform_textureIn(GLuint program, sb_image_t *image)
{
    sb_log_debug("_set_uniform_textureIn() - %ldx%ld\n",
                 sb_image_size(image)->width, sb_image_size(image)->height);

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
        GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        sb_image_size(image)->width,
        sb_image_size(image)->height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        sb_image_data(image)
    );
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
}

static GLuint _set_texture(sb_surface_t *surface)
{
    GLuint texture;
/*
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
        GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        surface->_size.width * surface->scale,
        surface->_size.height * surface->scale,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        sb_skia_context_buffer(surface->skia_context)
    );
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
*/
    return texture;
}

static void _draw_recursive(sb_surface_t *surface,
                            sb_view_t *view)
{
    enum sb_view_fill_type fill_type = sb_view_fill_type(view);

    if (fill_type == SB_VIEW_FILL_TYPE_SINGLE_COLOR) {
        const sb_view_radius_t *radius = NULL;
        const sb_list_t *filters = NULL;
        {
            // Set radius or NULL.
            if (!sb_view_radius_is_zero(sb_view_radius(view))) {
                radius = sb_view_radius(view);
            }
            // Set filters or NULL.
            if (sb_list_length((sb_list_t*)sb_view_filters(view))) {
                filters = sb_view_filters(view);
            }
        }
        sb_skia_draw_rect2(
            surface->skia_renderer,
            sb_view_geometry(view),
            surface->scale,
            sb_view_color(view),
            radius,
            filters,
            sb_view_clip(view)
        );
    } else if (fill_type == SB_VIEW_FILL_TYPE_IMAGE) {
        sb_skia_draw_image(surface->skia_renderer,
            sb_view_geometry(view), surface->scale, sb_view_image(view));
    }

    // Child views.
    sb_list_t *children = sb_view_children(view);
    for (int i = 0; i < sb_list_length(children); ++i) {
        const sb_view_radius_t *radius = NULL;
        if (!sb_view_radius_is_zero(sb_view_radius(view))) {
            radius = sb_view_radius(view);
        }

        if (sb_view_clip(view)) {
            // Clip parent view.
            sb_skia_clip_rect(surface->skia_renderer,
                sb_view_geometry(view),
                radius,
                surface->scale
            );
        }

        if (sb_view_parent(view) != NULL) {
            const sb_point_t view_pos = sb_view_geometry(view)->pos;
            sb_point_t scaled_pos;
            scaled_pos.x = view_pos.x * surface->scale;
            scaled_pos.y = view_pos.y * surface->scale;
            sb_skia_save_pos(surface->skia_renderer, &scaled_pos);
        }

        sb_view_t *child = sb_list_at(children, i);
        _draw_recursive(surface, child);

        if (sb_view_clip(view)) {
            // Restore parent view clip.
            sb_skia_clip_restore(surface->skia_renderer);
        }

        if (sb_view_parent(view) != NULL) {
            sb_skia_restore_pos(surface->skia_renderer);
        }
    }
}

void _add_frame_callback(sb_surface_t *surface)
{
    if (surface->frame_callback != NULL) {
        sb_log_warn("Frame callback is not NULL!\n");
        return;
    }
    surface->frame_callback = wl_surface_frame(surface->_wl_surface);
    wl_callback_add_listener(surface->frame_callback, &callback_listener,
        (void*)surface);
    sb_log_debug(" = frame_callback now %p\n", surface->frame_callback);
}

void _draw_frame(sb_surface_t *surface)
{
    sb_log_debug(" == _draw_frame() - surface: %p ==\n", surface);
    sb_application_t *app = sb_application_instance();

    enum sb_skia_backend backend = sb_skia_renderer_backend(surface->skia_renderer);
    if (backend == SB_SKIA_BACKEND_GL) {
        sb_skia_gl_renderer_t *renderer =
            sb_skia_renderer_current(surface->skia_renderer);
        sb_egl_context_t *egl_context = sb_application_egl_context(app);

        sb_skia_gl_renderer_begin(renderer,
            egl_context,
            surface->_egl_surface,
            surface->_size.width * surface->scale,
            surface->_size.height * surface->scale
        );

        sb_color_t clear_color = { 0x00, 0x0, 0x00, 0x00 };
        sb_skia_clear(surface->skia_renderer, &clear_color);

        _draw_recursive(surface, surface->_root_view);

        sb_skia_gl_renderer_end(renderer);

        eglSwapBuffers(egl_context->egl_display, surface->_egl_surface);
    }

    if (backend == SB_SKIA_BACKEND_RASTER) {
        sb_log_warn("raster backend not implemented!\n");
    }

#if 0
    // Skia context begin.
    sb_skia_context_set_buffer_size(surface->skia_context,
        surface->_size.width * surface->scale,
        surface->_size.height * surface->scale);
    sb_skia_context_begin(surface->skia_context,
        surface->_size.width * surface->scale,
        surface->_size.height * surface->scale);

    // Clear color.
    sb_color_t clear_color = { 0x00, 0x00, 0x00, 0x00 };
    sb_skia_clear(surface->skia_context, &clear_color);

    _draw_recursive(surface, surface->_root_view);

    // Skia context end.
    sb_skia_context_end(surface->skia_context);

    // Late make current if OpenGL backend.
    if (backend == SB_SKIA_BACKEND_GL) {
        _gl_init(surface);
    }

    // Compile shaders and attach to the program.
    if (surface->gl.vert_shader == 0) {
        surface->gl.vert_shader = _load_shader(canvas_vert_shader,
            GL_VERTEX_SHADER);
    }
    if (surface->gl.frag_shader == 0) {
        surface->gl.frag_shader = _load_shader(canvas_frag_shader,
            GL_FRAGMENT_SHADER);
    }
    // Create the program if not exist.
    if (surface->gl.program == 0) {
        surface->gl.program = glCreateProgram();
        glAttachShader(surface->gl.program, surface->gl.vert_shader);
        glAttachShader(surface->gl.program, surface->gl.frag_shader);

        // Link the program.
        glLinkProgram(surface->gl.program);
    }
    // Use the program.
    glUseProgram(surface->gl.program);

    // GL draw.
    // Set texture.
    GLuint texture = _set_texture(surface);

    // Set coordinates.
    float vertices[] = {
         1.0f,  1.0f, 0.0f,
         1.0f, -1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
    };

    GLuint indices[] = {
        0, 1, 3,
        1, 2, 3,
    };

    float tex_coord[] = {
        -1.0f,  0.0f,
        -1.0f, -1.0f,
         0.0f, -1.0f,
         0.0f,  0.0f,
    };

    // VAO.
    GLuint vao;
    glGenVertexArrays(1, &vao);

    // EBO.
    GLuint ebo;
    glGenBuffers(1, &ebo);

    // VBO.
    GLuint vbo[2];
    glGenBuffers(2, vbo);

    // Bind and draw.
    glBindVertexArray(vao);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
        GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER,
        sizeof(vertices),
        vertices,
        GL_STATIC_DRAW
    );

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER,
        sizeof(tex_coord),
        tex_coord,
        GL_STATIC_DRAW);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(1);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);

    // Swap buffers.
    eglSwapBuffers(surface->egl_context->egl_display, surface->_egl_surface);

    // Delete texture.
    glDeleteTextures(1, &texture);

    // Delete program.
    // glDeleteProgram(surface->gl.program);
    // surface->gl.program = 0;
#endif
}

//!<===============
//!< Surface
//!<===============

sb_surface_t* sb_surface_new()
{
    sb_surface_t *surface = malloc(sizeof(sb_surface_t));

    surface->_size.width = 200.0f;
    surface->_size.height = 200.0f;
    surface->frame_ready = false;
    surface->update_pending = false;
    surface->frame_callback = NULL;

    sb_application_t *app = sb_application_instance();

    // Create wl_surface.
    surface->_wl_surface = wl_compositor_create_surface(
        sb_application_wl_compositor(app));

    // Add surface listener.
    wl_surface_add_listener(surface->_wl_surface, &surface_listener,
        (void*)surface);

    // Get global EGL context.
    sb_egl_context_t *egl_context = sb_application_egl_context(app);

    // Create wl_egl_window.
    surface->_wl_egl_window = wl_egl_window_create(surface->_wl_surface,
       surface->_size.width,
       surface->_size.height);

    // Create EGL surface.
    surface->_egl_surface = eglCreateWindowSurface(
        egl_context->egl_display,
        egl_context->egl_config,
        surface->_wl_egl_window,
        NULL);

    EGLBoolean res = eglMakeCurrent(
        egl_context->egl_display,
        surface->_egl_surface,
        surface->_egl_surface,
        egl_context->egl_context
    );
    if (!res) {
        EGLint err = eglGetError();
        sb_log_warn("eglMakeCurrent failed in NEW. 0x%x", err);
    }

    // Detect Swingby rendering backend.
    const char *backend = getenv("SWINGBY_BACKEND");
    if (backend == NULL) {
        backend = SWINGBY_BACKEND_DEFAULT;
    }

    if (strcmp(backend, "opengl") == 0) {
        surface->skia_renderer = sb_skia_renderer_new(SB_SKIA_BACKEND_GL);
    } else if (strcmp(backend, "raster") == 0) {
        surface->skia_renderer = sb_skia_renderer_new(SB_SKIA_BACKEND_RASTER);
    } else {
        sb_log_warn("sb_surface_new() - Invalid backend.\n");
    }

    // Initialize the program objects.
    surface->programs.color = 0;
    surface->programs.texture = 0;

    surface->gl.vert_shader = 0;
    surface->gl.frag_shader = 0;
    surface->gl.program = 0;

    // Root view.
    sb_rect_t geo;
    geo.pos.x = 0.0f;
    geo.pos.y = 0.0f;
    geo.size.width = surface->_size.width;
    geo.size.height = surface->_size.height;
    surface->_root_view = sb_view_new(NULL, &geo);
    sb_view_set_surface(surface->_root_view, surface);

    // Scale.
    surface->scale = 1;

    // Focused view.
    surface->focused_view = NULL;

    // Event listeners.
    surface->event_listeners = sb_list_new();

    sb_log_debug("sb_surface_new() - surface: %p\n", surface);
    sb_log_debug("sb_surface_new() - root_view: %p\n", surface->_root_view);
    return surface;
}

void sb_surface_set_wl_surface(sb_surface_t *surface,
                               struct wl_surface *wl_surface)
{
    surface->_wl_surface = wl_surface;
}

const sb_size_t* sb_surface_size(sb_surface_t *surface)
{
    return &surface->_size;
}

void sb_surface_set_size(sb_surface_t *surface, const sb_size_t *size)
{
    if (size->width <= 0.0f || size->height <= 0.0f) {
        sb_log_warn("Surface size cannot be zero or negative.\n");
    }

    surface->_size.width = size->width;
    surface->_size.height = size->height;

    // Set the root view's size.
    sb_rect_t new_geo;
    new_geo.pos.x = 0;
    new_geo.pos.y = 0;
    new_geo.size.width = size->width;
    new_geo.size.height = size->height;
    sb_view_set_geometry(surface->_root_view, &new_geo);

    // Create event.
    sb_event_t *event = sb_event_new(SB_EVENT_TARGET_TYPE_SURFACE,
        (void*)surface,
        SB_EVENT_TYPE_RESIZE);
    event->resize.old_size = *sb_surface_size(surface);
    event->resize.size.width = size->width;
    event->resize.size.height = size->height;

    // Post the event.
    sb_application_post_event(sb_application_instance(), event);

    sb_surface_update(surface);

    wl_egl_window_resize(surface->_wl_egl_window,
        surface->_size.width * surface->scale,
        surface->_size.height * surface->scale,
        0, 0);
}

sb_view_t* sb_surface_root_view(sb_surface_t *surface)
{
    return surface->_root_view;
}

void sb_surface_commit(sb_surface_t *surface)
{
    wl_surface_commit(surface->_wl_surface);
}

void sb_surface_attach(sb_surface_t *surface)
{
    sb_log_debug("(sb_surface_attach) - surface: %p\n", surface);

    _add_frame_callback(surface);

    _draw_frame(surface);
}

void sb_surface_detach(sb_surface_t *surface)
{
    wl_surface_attach(surface->_wl_surface, NULL, 0, 0);
    wl_surface_commit(surface->_wl_surface);

    // Destroy frame callback.
    wl_callback_destroy(surface->frame_callback);
    surface->frame_callback = NULL;
}

void sb_surface_update(sb_surface_t *surface)
{
    if (surface->frame_ready == true) {
        // Post request update event.
        sb_event_t *event = sb_event_new(SB_EVENT_TARGET_TYPE_SURFACE,
            (void*)surface, SB_EVENT_TYPE_REQUEST_UPDATE);
        sb_application_post_event(sb_application_instance(), event);

        surface->frame_ready = false;
    } else {
        sb_log_debug("sb_surface_update() - Frame not ready!\n");
        surface->update_pending = true;
    }
}

uint32_t sb_surface_scale(const sb_surface_t *surface)
{
    return surface->scale;
}

void sb_surface_set_scale(sb_surface_t *surface, uint32_t scale)
{
    surface->scale = scale;
    wl_surface_set_buffer_scale(surface->_wl_surface, scale);

    wl_egl_window_resize(surface->_wl_egl_window,
        surface->_size.width * surface->scale,
        surface->_size.height * surface->scale,
        0, 0);

    sb_surface_update(surface);
}

void sb_surface_set_input_geometry(sb_surface_t *surface, sb_rect_t *geometry)
{
    sb_application_t *app = sb_application_instance();
    struct wl_surface *wl_surface = sb_surface_wl_surface(surface);
    struct wl_compositor *wl_compositor = sb_application_wl_compositor(app);

    struct wl_region *region = wl_compositor_create_region(wl_compositor);
    wl_region_add(region,
        geometry->pos.x, geometry->pos.y,
        geometry->size.width, geometry->size.height);
    wl_surface_set_input_region(wl_surface, region);
    wl_region_destroy(region);
}

void sb_surface_enable_text_input(sb_surface_t *surface,
                                  const sb_rect_t *rect)
{
    sb_application_t *app = sb_application_instance();

    struct zwp_text_input_v3 *text_input =
        sb_application_zwp_text_input_v3(app);

    zwp_text_input_v3_enable(text_input);
    zwp_text_input_v3_set_cursor_rectangle(text_input,
        rect->pos.x, rect->pos.y,
        rect->size.width, rect->size.height);
    zwp_text_input_v3_commit(text_input);
}

void sb_surface_disable_text_input(sb_surface_t *surface)
{
    sb_application_t *app = sb_application_instance();

    struct zwp_text_input_v3 *text_input =
        sb_application_zwp_text_input_v3(app);

    zwp_text_input_v3_disable(text_input);
    zwp_text_input_v3_commit(text_input);
}

sb_view_t* sb_surface_focused_view(const sb_surface_t *surface)
{
    return surface->focused_view;
}

void sb_surface_set_focused_view(sb_surface_t *surface, sb_view_t *view)
{
    surface->focused_view = view;
}

void sb_surface_free(sb_surface_t *surface)
{
    wl_egl_window_destroy(surface->_wl_egl_window);
    wl_surface_destroy(surface->_wl_surface);

    // TODO: Free the views.

    free(surface);
}

void sb_surface_add_event_listener(sb_surface_t *surface,
                                   enum sb_event_type event_type,
                                   sb_event_listener_t listener,
                                   void *user_data)
{
    sb_event_listener_tuple_t *tuple = sb_event_listener_tuple_new(
        event_type, listener, user_data);
    sb_list_push(surface->event_listeners, (void*)tuple);
}

void sb_surface_on_pointer_enter(sb_surface_t *surface, sb_event_t *event)
{
    _event_listener_filter_for_each(surface->event_listeners,
        SB_EVENT_TYPE_POINTER_ENTER, event);
}

void sb_surface_on_pointer_leave(sb_surface_t *surface, sb_event_t *event)
{
    _event_listener_filter_for_each(surface->event_listeners,
        SB_EVENT_TYPE_POINTER_LEAVE, event);
}

void sb_surface_on_request_update(sb_surface_t *surface)
{
    sb_log_debug("sb_surface_on_request_update()\n");

    _draw_frame(surface);

    // Set frame ready flag to false.
    surface->frame_ready = false;
}

void sb_surface_on_resize(sb_surface_t *surface, sb_event_t *event)
{
    _event_listener_filter_for_each(surface->event_listeners,
        SB_EVENT_TYPE_RESIZE, event);
}

void sb_surface_on_keyboard_key_press(sb_surface_t *surface,
                                      sb_event_t *event)
{
    _event_listener_filter_for_each(surface->event_listeners,
        SB_EVENT_TYPE_KEYBOARD_KEY_PRESS, event);
}

void sb_surface_on_keyboard_key_release(sb_surface_t *surface,
                                        sb_event_t *event)
{
    _event_listener_filter_for_each(surface->event_listeners,
        SB_EVENT_TYPE_KEYBOARD_KEY_RELEASE, event);
}

void sb_surface_on_preferred_scale(sb_surface_t *surface,
                                   sb_event_t *event)
{
    _event_listener_filter_for_each(surface->event_listeners,
        SB_EVENT_TYPE_PREFERRED_SCALE, event);
}

void sb_surface_on_timeout(sb_surface_t *surface,
                           sb_event_t *event)
{
    _event_listener_filter_for_each(surface->event_listeners,
        SB_EVENT_TYPE_TIMEOUT, event);
}

struct wl_surface* sb_surface_wl_surface(sb_surface_t *surface)
{
    return surface->_wl_surface;
}

//!<=================
//!< Callback
//!<=================

static void callback_done_handler(void *data,
                                  struct wl_callback *wl_callback,
                                  uint32_t time)
{
    sb_surface_t *surface = (sb_surface_t*)data;
    sb_log_debug(" = callback_done_handler - surface: %p\n", surface);

    wl_callback_destroy(surface->frame_callback);
    surface->frame_callback = NULL;
    _add_frame_callback(surface);

    surface->frame_ready = true;
    sb_log_debug(" == FRAME READY ==\n");

    if (surface->update_pending) {
        sb_log_debug(" == THERE ARE PENDING UPDATES ==\n");
        _draw_frame(surface);
        surface->update_pending = false;
        surface->frame_ready = false;
    }
}

//!<====================
//!< Wayland Surface
//!<====================

static void enter_handler(void *data,
                          struct wl_surface *wl_surface,
                          struct wl_output *wl_output)
{
}

static void leave_handler(void *data,
                          struct wl_surface *wl_surface,
                          struct wl_output *wl_output)
{
}

static void preferred_buffer_scale_handler(void *data,
                                           struct wl_surface *wl_surface,
                                           int32_t factor)
{
    sb_surface_t *surface = (sb_surface_t*)data;
    sb_log_debug("preferred_buffer_scale_handler - factor: %d\n", factor);

    sb_event_t *event = sb_event_new(SB_EVENT_TARGET_TYPE_SURFACE, surface,
        SB_EVENT_TYPE_PREFERRED_SCALE);
    event->scale.scale = factor;

    sb_application_t *app = sb_application_instance();
    sb_application_post_event(app, event);
}

static void preferred_buffer_transform_handler(void *data,
                                               struct wl_surface *wl_surface,
                                               uint32_t transform)
{
}
