#include <foundation/surface.h>

#include <stdbool.h>
#include <stdlib.h>

#include <sys/time.h>

#include <wayland-client.h>
#include <wayland-egl.h>

#include <EGL/egl.h>
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>

#include <foundation/log.h>
#include <foundation/bench.h>
#include <foundation/application.h>
#include <foundation/egl-context.h>
#include <foundation/view.h>
#include <foundation/image.h>
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
    bool frame_ready;
    bool update_pending;
    /// \brief Program objects for OpenGL.
    struct {
        GLuint color;
        GLuint texture;
    } programs;
    struct wl_callback *frame_callback;
    ft_list_t *event_listeners;
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

    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,
        GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

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

static void _set_uniform_textureIn(GLuint program, ft_image_t *image)
{
    ft_log_debug("_set_uniform_textureIn() - %ldx%ld\n",
                 ft_image_size(image)->width, ft_image_size(image)->height);

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
        ft_image_size(image)->width,
        ft_image_size(image)->height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        ft_image_data(image)
    );
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
}

static void _draw_recursive(ft_surface_t *surface,
                            ft_view_t *view)
{
    enum ft_view_fill_type fill_type = ft_view_fill_type(view);

    // Create program if not created.
    if (fill_type == FT_VIEW_FILL_TYPE_SINGLE_COLOR &&
        surface->programs.color == 0) {
        // Create program object.
        surface->programs.color = glCreateProgram();

        // Create shaders.
        GLuint vert_shader = _load_shader(rect_vert_shader, GL_VERTEX_SHADER);
        GLuint frag_shader = _load_shader(color_frag_shader,
            GL_FRAGMENT_SHADER);

        // Attach shaders.
        glAttachShader(surface->programs.color, vert_shader);
        glAttachShader(surface->programs.color, frag_shader);
    }
    if (fill_type == FT_VIEW_FILL_TYPE_IMAGE &&
        surface->programs.texture == 0) {
        // Create program object.
        surface->programs.texture = glCreateProgram();

        // Create shaders.
        GLuint vert_shader = _load_shader(rect_vert_shader, GL_VERTEX_SHADER);
        GLuint frag_shader = _load_shader(texture_frag_shader,
            GL_FRAGMENT_SHADER);

        // Attach shaders.
        glAttachShader(surface->programs.texture, vert_shader);
        glAttachShader(surface->programs.texture, frag_shader);
    }

    if (fill_type == FT_VIEW_FILL_TYPE_SINGLE_COLOR) {
        glLinkProgram(surface->programs.color);
        glUseProgram(surface->programs.color);

        // Set uniforms.
        _set_uniform_resolution(surface->programs.color, &surface->_size);
        _set_uniform_color(surface->programs.color, ft_view_color(view));
    } else if (fill_type == FT_VIEW_FILL_TYPE_IMAGE) {
        glLinkProgram(surface->programs.texture);
        glUseProgram(surface->programs.texture);

        // Set uniforms.
        _set_uniform_resolution(surface->programs.texture, &surface->_size);
        _set_uniform_textureIn(surface->programs.texture,
            ft_view_image(view));
    }

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

    float tex_coord[] = {
        0.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
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

    // Child views.
    ft_list_t *children = ft_view_children(view);
    for (int i = 0; i < ft_list_length(children); ++i) {
        ft_view_t *child = ft_list_at(children, i);
        _draw_recursive(surface, child);
    }
}

void _add_frame_callback(ft_surface_t *surface)
{
    surface->frame_callback = wl_surface_frame(surface->_wl_surface);
    wl_callback_add_listener(surface->frame_callback, &callback_listener,
        (void*)surface);
    // ft_log_debug(" = frame_callback now %p\n", surface->frame_callback);
}

void _draw_frame(ft_surface_t *surface)
{
    _gl_init(surface);

    eglMakeCurrent(surface->_egl_context->egl_display,
        surface->_egl_surface, surface->_egl_surface,
        surface->_egl_context->egl_context);

    _draw_recursive(surface, surface->_root_view);

    eglSwapBuffers(surface->_egl_context->egl_display, surface->_egl_surface);
}

static void _event_listener_filter_for_each(ft_list_t *listeners,
                                            enum ft_event_type type,
                                            ft_event_t *event)
{
    uint64_t length = ft_list_length(listeners);
    for (uint64_t i = 0; i < length; ++i) {
        ft_event_listener_tuple_t *tuple = ft_list_at(listeners, i);
        if (tuple->type == type) {
            tuple->listener(event);
        }
    }
}

//!<===============
//!< Surface
//!<===============

ft_surface_t* ft_surface_new()
{
    ft_surface_t *surface = malloc(sizeof(ft_surface_t));

    surface->_size.width = 200.0f;
    surface->_size.height = 200.0f;
    surface->frame_ready = false;
    surface->update_pending = false;

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

    // Initialize the program objects.
    surface->programs.color = 0;
    surface->programs.texture = 0;

    // Root view.
    ft_rect_t geo;
    geo.pos.x = 0.0f;
    geo.pos.y = 0.0f;
    geo.size.width = surface->_size.width;
    geo.size.height = surface->_size.height;
    surface->_root_view = ft_view_new(NULL, &geo);
    ft_view_set_surface(surface->_root_view, surface);

    // Event listeners.
    surface->event_listeners = ft_list_new();

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
    if (size->width <= 0.0f || size->height <= 0.0f) {
        ft_log_warn("Surface size cannot be zero or negative.\n");
    }

    surface->_size.width = size->width;
    surface->_size.height = size->height;

    // Set the root view's size.
    ft_rect_t new_geo;
    new_geo.pos.x = 0;
    new_geo.pos.y = 0;
    new_geo.size.width = size->width;
    new_geo.size.height = size->height;
    ft_view_set_geometry(surface->_root_view, &new_geo);

    // Create event.
    ft_event_t *event = ft_event_new(FT_EVENT_TARGET_TYPE_SURFACE,
        (void*)surface,
        FT_EVENT_TYPE_RESIZE);
    event->resize.old_size = *ft_surface_size(surface);
    event->resize.size.width = size->width;
    event->resize.size.height = size->height;

    // Post the event.
    ft_application_post_event(ft_application_instance(), event);

    ft_surface_update(surface);

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
    if (surface->frame_ready == true) {
        // Post request update event.
        ft_event_t *event = ft_event_new(FT_EVENT_TARGET_TYPE_SURFACE,
            (void*)surface, FT_EVENT_TYPE_REQUEST_UPDATE);
        ft_application_post_event(ft_application_instance(), event);

        surface->frame_ready = false;
    } else {
        ft_log_debug("ft_surface_update() - Frame not ready!\n");
        surface->update_pending = true;
    }
}

void ft_surface_set_input_geometry(ft_surface_t *surface, ft_rect_t *geometry)
{
    ft_application_t *app = ft_application_instance();
    struct wl_surface *wl_surface = ft_surface_wl_surface(surface);
    struct wl_compositor *wl_compositor = ft_application_wl_compositor(app);

    struct wl_region *region = wl_compositor_create_region(wl_compositor);
    wl_region_add(region,
        geometry->pos.x, geometry->pos.y,
        geometry->size.width, geometry->size.height);
    wl_surface_set_input_region(wl_surface, region);
    wl_region_destroy(region);
}

void ft_surface_add_event_listener(ft_surface_t *surface,
                                   enum ft_event_type event_type,
                                   void (*listener)(ft_event_t*))
{
    ft_event_listener_tuple_t *tuple = ft_event_listener_tuple_new(
        event_type, listener);
    ft_list_push(surface->event_listeners, (void*)tuple);
}

void ft_surface_on_pointer_enter(ft_surface_t *surface, ft_event_t *event)
{
    _event_listener_filter_for_each(surface->event_listeners,
        FT_EVENT_TYPE_POINTER_ENTER, event);
}

void ft_surface_on_pointer_leave(ft_surface_t *surface, ft_event_t *event)
{
    _event_listener_filter_for_each(surface->event_listeners,
        FT_EVENT_TYPE_POINTER_LEAVE, event);
}

void ft_surface_on_request_update(ft_surface_t *surface)
{
    ft_log_debug("ft_surface_on_request_update()\n");

    _draw_frame(surface);

    // Set frame ready flag to false.
    surface->frame_ready = false;
}

void ft_surface_on_resize(ft_surface_t *surface, ft_event_t *event)
{
    _event_listener_filter_for_each(surface->event_listeners,
        FT_EVENT_TYPE_RESIZE, event);
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

    wl_callback_destroy(wl_callback);
    _add_frame_callback(surface);

    surface->frame_ready = true;
    ft_log_debug(" == FRAME READY ==\n");

    if (surface->update_pending) {
        ft_log_debug(" == THERE ARE PENDING UPDATES ==\n");
        _draw_frame(surface);
        surface->update_pending = false;
        surface->frame_ready = false;
    }
}
