#include <foundation/surface.h>

#include <stdbool.h>
#include <stdlib.h>

#include <wayland-client.h>
#include <wayland-egl.h>

#include <EGL/egl.h>
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>

#include <foundation/log.h>
#include <foundation/application.h>
#include <foundation/egl-context.h>
#include <foundation/view.h>
#include <foundation/list.h>
#include <foundation/event.h>
#include "shaders.h"

struct ft_surface_t {
    struct wl_surface *_wl_surface;
    struct wl_egl_window *_wl_egl_window;
    EGLSurface _egl_surface;
    ft_egl_context_t *_egl_context;
    ft_size_t _size;
    ft_view_t *_root_view;
    bool updated;
    struct wl_callback *frame_callback;
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

//!<======================
//!< Helper Functions
//!<======================

void _gl_init(ft_surface_t *surface)
{
    eglMakeCurrent(surface->_egl_context->egl_display,
        surface->_egl_surface,
        surface->_egl_surface,
        surface->_egl_context->egl_context);

    glViewport(0, 0, surface->_size.width, surface->_size.height);

    glClearColor(0.5, 0.5, 0.5, 0.5);
    glClear(GL_COLOR_BUFFER_BIT);

    // glUseProgram();

    // eglSwapBuffers(surface->_egl_context->egl_display, surface->_egl_surface);
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
static void _calc_points(const ft_rect_t *rect, float *points)
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
static void _set_uniform_resolution(GLuint program, const ft_size_t *resolution)
{
    GLuint location = glGetUniformLocation(program, "resolution");
    float resolution_u[2] = { resolution->width, resolution->height };
    glUniform2fv(location, 1, resolution_u);
}

/// \brief Set the uniform variable `color`.
static void _set_uniform_color(GLuint program, const ft_color_t *color)
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

static void _draw_recursive(GLuint program,
                            ft_view_t *view,
                            GLuint vao,
                            GLuint ebo,
                            GLuint vbo)
{
    _set_uniform_color(program, ft_view_color(view));

    // Set coordinates.
    float vertices[] = {
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
    };
    ft_rect_t absolute_geometry;
    absolute_geometry = *ft_view_geometry(view);

    ft_view_t *parent = ft_view_parent(view);
    for (ft_view_t *it = parent; it != NULL; it = ft_view_parent(it)) {
        absolute_geometry.pos.x += ft_view_geometry(it)->pos.x;
        absolute_geometry.pos.y += ft_view_geometry(it)->pos.y;
    }
    _calc_points(&absolute_geometry, vertices);

    GLuint indices[] = {
        0, 1, 3,
        1, 2, 3,
    };

    // Bind and draw.
    glBindVertexArray(vao);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
        GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER,
        sizeof(vertices),
        vertices,
        GL_STATIC_DRAW
    );

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);

    // Child views.
    ft_list_t *children = ft_view_children(view);
    for (int i = 0; i < ft_list_length(children); ++i) {
        ft_view_t *child = ft_list_at(children, i);
        _draw_recursive(program, child, vao, ebo, vbo);
    }
}

void _add_frame_callback(ft_surface_t *surface)
{
    surface->frame_callback = wl_surface_frame(surface->_wl_surface);
    wl_callback_add_listener(surface->frame_callback, &callback_listener,
        (void*)surface);
    ft_log_debug(" = frame_callback now %p\n", surface->frame_callback);
}

//!<===============
//!< Surface
//!<===============

ft_surface_t* ft_surface_new()
{
    ft_surface_t *surface = malloc(sizeof(ft_surface_t));

    surface->_size.width = 200.0f;
    surface->_size.height = 200.0f;
    surface->updated = false;

    ft_application_t *app = ft_application_instance();

    // Create wl_surface.
    surface->_wl_surface = wl_compositor_create_surface(
        ft_application_wl_compositor(app));

    // Frame callback.
    surface->frame_callback = wl_surface_frame(surface->_wl_surface);
    wl_callback_add_listener(surface->frame_callback, &callback_listener,
        (void*)surface);

    // Initialize EGL context.
    surface->_egl_context = ft_egl_context_new();

    // Create wl_egl_window.
    surface->_wl_egl_window = wl_egl_window_create(surface->_wl_surface,
       surface->_size.width,
       surface->_size.height);

    // Create EGL surface.
    surface->_egl_surface = eglCreateWindowSurface(
        surface->_egl_context->egl_display,
        surface->_egl_context->egl_config,
        surface->_wl_egl_window,
        NULL);

    // Root view.
    ft_rect_t geo;
    geo.pos.x = 0.0f;
    geo.pos.y = 0.0f;
    geo.size.width = surface->_size.width;
    geo.size.height = surface->_size.height;
    surface->_root_view = ft_view_new(NULL, &geo);
    ft_view_set_surface(surface->_root_view, surface);

    ft_log_debug("ft_surface_new() - surface: %p\n", surface);
    ft_log_debug("ft_surface_new() - root_view: %p\n", surface->_root_view);
    return surface;
}

void ft_surface_set_wl_surface(ft_surface_t *surface,
                               struct wl_surface *wl_surface)
{
    surface->_wl_surface = wl_surface;
}

const ft_size_t* ft_surface_size(ft_surface_t *surface)
{
    return &surface->_size;
}

void ft_surface_set_size(ft_surface_t *surface, const ft_size_t *size)
{
    surface->_size.width = size->width;
    surface->_size.height = size->height;

    wl_egl_window_resize(surface->_wl_egl_window, size->width, size->height,
        0, 0);
}

ft_view_t* ft_surface_root_view(ft_surface_t *surface)
{
    return surface->_root_view;
}

void ft_surface_commit(ft_surface_t *surface)
{
    wl_surface_commit(surface->_wl_surface);
}

void ft_surface_attach(ft_surface_t *surface)
{
    _gl_init(surface);
    eglSwapBuffers(surface->_egl_context->egl_display, surface->_egl_surface);
}

void ft_surface_detach(ft_surface_t *surface)
{
    // eglDestroySurface(surface->_egl_context->egl_display,
    //     surface->_egl_surface);

    // wl_egl_window_destroy(surface->_wl_egl_window);

    // ft_egl_context_free(surface->_egl_context);

    wl_surface_attach(surface->_wl_surface, NULL, 0, 0);
    wl_surface_commit(surface->_wl_surface);
}

void ft_surface_update(ft_surface_t *surface)
{
    surface->updated = false;

    _add_frame_callback(surface);

    // Post request update event.
    ft_event_t *event = ft_event_new(FT_EVENT_TARGET_TYPE_SURFACE,
        (void*)surface, FT_EVENT_TYPE_REQUEST_UPDATE);
    ft_application_post_event(ft_application_instance(), event);
}

void ft_surface_on_pointer_enter(ft_surface_t *surface, ft_event_t *event)
{
    //
}

void ft_surface_on_request_update(ft_surface_t *surface)
{
    // Create program.
    GLuint program = glCreateProgram();

    // Create shaders.
    GLuint vert_shader = _load_shader(rect_vert_shader, GL_VERTEX_SHADER);
    GLuint frag_shader = _load_shader(color_frag_shader, GL_FRAGMENT_SHADER);

    // Attach shaders.
    glAttachShader(program, vert_shader);
    glAttachShader(program, frag_shader);

    glLinkProgram(program);

    glUseProgram(program);

    // Set uniforms.
    _set_uniform_resolution(program, &surface->_size);
    _set_uniform_color(program, ft_view_color(surface->_root_view));

    eglMakeCurrent(surface->_egl_context->egl_display,
        surface->_egl_surface, surface->_egl_surface,
        surface->_egl_context->egl_context);

    // VAO.
    GLuint vao;
    glGenVertexArrays(1, &vao);

    // EBO.
    GLuint ebo;
    glGenBuffers(1, &ebo);

    // VBO.
    GLuint vbo;
    glGenBuffers(1, &vbo);

    _draw_recursive(program, surface->_root_view, vao, ebo, vbo);

    eglSwapBuffers(surface->_egl_context->egl_display, surface->_egl_surface);

    // Set updated flag to true.
    surface->updated = true;
}

struct wl_surface* ft_surface_wl_surface(ft_surface_t *surface)
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
    ft_surface_t *surface = (ft_surface_t*)data;
    ft_log_debug("callback_done_handler() %p %p\n",
                 wl_callback, surface->frame_callback);

    // Post request update event.
    if (surface->updated == false) {
        ft_event_t *event = ft_event_new(FT_EVENT_TARGET_TYPE_SURFACE,
            (void*)surface, FT_EVENT_TYPE_REQUEST_UPDATE);
        ft_application_post_event(ft_application_instance(), event);
    }

    wl_callback_destroy(wl_callback);
}
