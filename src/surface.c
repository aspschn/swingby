#include <swingby/surface.h>

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/time.h>
#include <sys/mman.h>

#include <wayland-client.h>
#include <wayland-egl.h>
#include <wayland-protocols/stable/viewporter.h>
#include <wayland-protocols/staging/fractional-scale-v1.h>
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
#include <swingby/canvas.h>
#include <swingby/list.h>
#include <swingby/event.h>

#include "helpers/shared.h"
#include "./helpers/surface.h"

#include "egl-context/egl-context.h"

#include "skia/renderer.h"
#include "skia/raster-renderer.h"
#include "skia/gl-renderer.h"
#include "skia/draw.h"

// #define SWINGBY_BACKEND_DEFAULT "raster"
#define SWINGBY_BACKEND_DEFAULT "opengl"

struct sb_surface_t {
    struct wl_surface *_wl_surface;
    struct wl_egl_window *_wl_egl_window;
    EGLSurface _egl_surface;
    struct wl_shm_pool *wl_shm_pool;
    struct wl_buffer *wl_buffer;
    struct wp_fractional_scale_v1 *wp_fractional_scale_v1;
    struct {
        void *addr;
        uint32_t size;
    } shm_data;
    const char *backend;
    // sb_egl_context_t *egl_context;
    sb_skia_renderer_t *skia_renderer;
    sb_size_t _size;
    sb_view_t *_root_view;
    uint32_t scale;
    float scale_f;
    bool is_fractional_scale;
    sb_view_t *focused_view;
    /// Currently only one GL view supported.
    struct {
        GLuint fbo;
        GLuint texture;
        GLuint depth_rbo;
    } gl_view;
    bool frame_ready;
    bool update_pending;
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
//!< Fractional Scale
//!<======================

static void preferred_scale_handler(void *data,
                                    struct wp_fractional_scale_v1 *wp_scale,
                                    uint32_t scale);

static const struct wp_fractional_scale_v1_listener scale_listener = {
    .preferred_scale = preferred_scale_handler,
};

//!<======================
//!< Helper Functions
//!<======================

static void _gl_view_init(sb_surface_t *surface, int width, int height)
{
    if (surface->gl_view.fbo != 0) {
        glDeleteFramebuffers(1, &surface->gl_view.fbo);
        glDeleteTextures(1, &surface->gl_view.texture);
        glDeleteRenderbuffers(1, &surface->gl_view.depth_rbo);
    }

    glGenFramebuffers(1, &surface->gl_view.fbo);
    glGenTextures(1, &surface->gl_view.texture);
    glGenRenderbuffers(1, &surface->gl_view.depth_rbo);

    // Texture.
    glBindTexture(GL_TEXTURE_2D, surface->gl_view.texture);
    glTexImage2D(GL_TEXTURE_2D, 0,
        GL_RGBA8,
        width, height,
        0,                  // border
        GL_RGBA,            // format
        GL_UNSIGNED_BYTE,   // type
        NULL
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Renderbuffer.
    glBindRenderbuffer(GL_RENDERBUFFER, surface->gl_view.depth_rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);

    // Framebuffer.
    glBindFramebuffer(GL_FRAMEBUFFER, surface->gl_view.fbo);
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D, surface->gl_view.texture,
        0   // level
    );
    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
        GL_RENDERBUFFER, surface->gl_view.depth_rbo
    );
}

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

static void _raster_init(sb_surface_t *surface)
{
    sb_application_t *app = sb_application_instance();
    struct wl_shm *wl_shm = sb_application_wl_shm(app);

    // Unmap if already mapped.
    if (surface->shm_data.addr != NULL) {
        munmap(surface->shm_data.addr, surface->shm_data.size);
    }

    // Calculate the size.
    uint32_t stride = surface->_size.width * surface->scale * 4;
    uint32_t size = surface->_size.height * surface->scale * stride;

    int fd = _create_tmpfile(size);
    if (fd == -1) {
        sb_log_error("Failed to create tmpfile.\n");
        return;
    }

    surface->shm_data.addr = mmap(NULL,
        size,
        PROT_READ | PROT_WRITE,
        MAP_SHARED,
        fd,
        0);
    if (surface->shm_data.addr == MAP_FAILED) {
        sb_log_error("Failed to mmap tmpfile.\n");
        return;
    }
    surface->shm_data.size = size;

    surface->wl_shm_pool = wl_shm_create_pool(wl_shm, fd, size);
    surface->wl_buffer = wl_shm_pool_create_buffer(surface->wl_shm_pool,
        0,
        surface->_size.width,
        surface->_size.height,
        stride,
        WL_SHM_FORMAT_ARGB8888);

    close(fd);

    wl_shm_pool_destroy(surface->wl_shm_pool);
    surface->wl_shm_pool = NULL;
}

static void _draw_recursive(sb_surface_t *surface,
                            sb_view_t *view)
{
    enum sb_view_render_type render_type = sb_view_render_type(view);

    if (sb_view_visible(view) == false) {
        return;
    }

    if (render_type == SB_VIEW_RENDER_TYPE_SINGLE_COLOR) {
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
        sb_skia_draw_rect3(
            surface->skia_renderer,
            view,
            surface->scale);
    } else if (render_type == SB_VIEW_RENDER_TYPE_IMAGE) {
        // sb_skia_draw_image2(surface->skia_renderer,
        //     sb_view_geometry(view), surface->scale, sb_view_image(view));
        sb_skia_draw_image3(surface->skia_renderer, view, surface->scale);
    } else if (render_type == SB_VIEW_RENDER_TYPE_GLYPHS) {
        sb_skia_draw_glyphs(
            surface->skia_renderer,
            sb_view_geometry(view),
            surface->scale,
            sb_view_glyph_layout(view)
        );
    } else if (render_type == SB_VIEW_RENDER_TYPE_CANVAS) {
        // Get canvas.
        void *renderer = sb_skia_renderer_current(surface->skia_renderer);
        sb_skia_gl_renderer_t *gl_renderer = (sb_skia_gl_renderer_t*)renderer;
        void *sk_canvas = sb_skia_gl_renderer_canvas(gl_renderer);

        // Set the canvas to the view.
        sb_canvas_t *canvas = sb_canvas_new(sk_canvas);
        {
            const sb_point_t *pos = &sb_view_geometry(view)->position;
            sb_canvas_set_position(canvas, pos);
        }
        sb_canvas_set_scale(canvas, surface->scale);
        sb_view_set_canvas(view, canvas);

        // Post paint event.
        sb_event_t *event = sb_event_new(SB_EVENT_TARGET_TYPE_VIEW,
            view, SB_EVENT_TYPE_PAINT);
        // sb_application_post_event(app, event);
        // This event must be consumed here. Not on event dispatcher.
        sb_view_on_paint(view, event);
        sb_event_free(event);

        sb_canvas_free(canvas);
        sb_view_set_canvas(view, NULL);
    } else if (render_type == SB_VIEW_RENDER_TYPE_GL) {
        const sb_rect_t *geometry = sb_view_geometry(view);

        // Get renderer.
        void *renderer = sb_skia_renderer_current(surface->skia_renderer);
        sb_skia_gl_renderer_t *gl_renderer = (sb_skia_gl_renderer_t*)renderer;

        // Flush and submit.
        sb_skia_gl_renderer_flush_and_submit(gl_renderer);

        // It matters. The reason why is IDK.
        glUseProgram(0);

        // Store GL values.
        GLint vp[4];
        glGetIntegerv(GL_VIEWPORT, vp);
        GLint fbo;
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &fbo);

        // Init GL FBO.
        _gl_view_init(surface, geometry->size.width, geometry->size.height);

        // Set GL context.
        glBindFramebuffer(GL_FRAMEBUFFER, surface->gl_view.fbo);

        glViewport(
            // geometry->position.x,
            // surface_size->height - geometry->position.y - geometry->size.height,
            0, 0,
            geometry->size.width,
            geometry->size.height
        );
        glEnable(GL_SCISSOR_TEST);
        glScissor(
            // geometry->position.x,
            // surface_size->height - geometry->position.y - geometry->size.height,
            0, 0,
            geometry->size.width,
            geometry->size.height
        );

        // This event must be consumed here. Same as PAINT event type.
        sb_event_t *event = sb_event_new(SB_EVENT_TARGET_TYPE_VIEW,
            view, SB_EVENT_TYPE_DIRECT_RENDER);
        sb_view_on_render(view, event);
        sb_event_free(event);

        // Restore GL values.
        glViewport(vp[0], vp[1], vp[2], vp[3]);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glDisable(GL_SCISSOR_TEST);

        //
        sb_skia_draw_texture(surface->skia_renderer,
            geometry,
            surface->scale,
            surface->gl_view.texture
        );

        // Reset context.
        sb_skia_gl_renderer_reset_context(gl_renderer);
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
            const sb_point_t view_pos = sb_view_geometry(view)->position;
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
        sb_egl_t *egl = sb_application_egl(app);

        sb_skia_gl_renderer_begin(renderer,
            egl,
            surface->_egl_surface,
            surface->_size.width * surface->scale,
            surface->_size.height * surface->scale
        );

        sb_color_t clear_color = { 0.0f, 0.0f, 0.0f, 0.0f };
        sb_skia_clear(surface->skia_renderer, &clear_color);

        _draw_recursive(surface, surface->_root_view);

        sb_skia_gl_renderer_end(renderer);

        eglSwapBuffers(egl->egl_display, surface->_egl_surface);
    }

    if (backend == SB_SKIA_BACKEND_RASTER) {
        uint32_t width = surface->_size.width * surface->scale;
        uint32_t height = surface->_size.height * surface->scale;
        uint32_t stride = width * 4;
        uint32_t size = height * stride;

        sb_skia_raster_renderer_t *renderer =
            sb_skia_renderer_current(surface->skia_renderer);

        sb_skia_raster_renderer_begin(renderer,
            surface->_size.width * surface->scale,
            surface->_size.height * surface->scale);

        sb_color_t clear_color = { 0x00, 0x00, 0x00, 0x00 };
        sb_skia_clear(surface->skia_renderer, &clear_color);

        _draw_recursive(surface, surface->_root_view);

        // Copy pixels.
        void *pixels = sb_skia_raster_renderer_buffer(renderer);
        memcpy(surface->shm_data.addr, pixels, size);

        sb_skia_raster_renderer_end(renderer);

        // wl_surface_commit(surface->_wl_surface);
    }
}

//!<===============
//!< Surface
//!<===============

sb_surface_t* sb_surface_new()
{
    sb_surface_t *surface = malloc(sizeof(sb_surface_t));

    surface->_size.width = 200.0f;
    surface->_size.height = 200.0f;
    surface->scale = 1;
    surface->scale_f = 1.0f;
    surface->is_fractional_scale = false;
    surface->frame_ready = false;
    surface->update_pending = false;
    surface->frame_callback = NULL;
    surface->wl_shm_pool = NULL;
    surface->shm_data.addr = NULL;
    surface->shm_data.size = 0;
    surface->backend = NULL;
    surface->gl_view.fbo = 0;
    surface->gl_view.texture = 0;
    surface->wp_fractional_scale_v1 = NULL;

    sb_application_t *app = sb_application_instance();

    // Create wl_surface.
    surface->_wl_surface = wl_compositor_create_surface(
        sb_application_wl_compositor(app));

    // Add surface listeners.
    wl_surface_add_listener(surface->_wl_surface, &surface_listener,
        (void*)surface);

    struct wp_fractional_scale_manager_v1 *scale_manager =
        sb_application_wp_fractional_scale_manager_v1(app);
    if (scale_manager != NULL) {
        surface->wp_fractional_scale_v1 =
            wp_fractional_scale_manager_v1_get_fractional_scale(
                scale_manager,
                surface->_wl_surface
            );

        wp_fractional_scale_v1_add_listener(surface->wp_fractional_scale_v1,
            &scale_listener, (void*)surface);
    }

    // Detect Swingby rendering backend.
    surface->backend = getenv("SWINGBY_BACKEND");
    if (surface->backend == NULL) {
        surface->backend = SWINGBY_BACKEND_DEFAULT;
    }

    if (strcmp(surface->backend, "opengl") == 0) {
        surface->skia_renderer = sb_skia_renderer_new(SB_SKIA_BACKEND_GL);
    } else if (strcmp(surface->backend, "raster") == 0) {
        surface->skia_renderer = sb_skia_renderer_new(SB_SKIA_BACKEND_RASTER);
    } else {
        sb_log_warn("sb_surface_new() - Invalid backend.\n");
    }

    if (strcmp(surface->backend, "opengl") == 0) {
        // Get global EGL context.
        sb_egl_t *egl = sb_application_egl(app);

        // Create wl_egl_window.
        surface->_wl_egl_window = wl_egl_window_create(surface->_wl_surface,
           surface->_size.width,
           surface->_size.height);

        // Create EGL surface.
        surface->_egl_surface = eglCreateWindowSurface(
            egl->egl_display,
            egl->egl_config,
            surface->_wl_egl_window,
            NULL);

        EGLBoolean res = eglMakeCurrent(
            egl->egl_display,
            surface->_egl_surface,
            surface->_egl_surface,
            egl->egl_context
        );
        if (!res) {
            EGLint err = eglGetError();
            sb_log_warn("eglMakeCurrent failed in NEW. 0x%x", err);
        }
    }

    if (strcmp(surface->backend, "raster") == 0) {
        _raster_init(surface);

        // _add_frame_callback(surface);
    }

    // Root view.
    sb_rect_t geo;
    geo.position.x = 0.0f;
    geo.position.y = 0.0f;
    geo.size.width = surface->_size.width;
    geo.size.height = surface->_size.height;
    surface->_root_view = sb_view_new(NULL, &geo);
    sb_view_set_surface(surface->_root_view, surface);

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
    new_geo.position.x = 0;
    new_geo.position.y = 0;
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

    if (strcmp(surface->backend, "opengl") == 0) {
        wl_egl_window_resize(surface->_wl_egl_window,
            surface->_size.width * surface->scale,
            surface->_size.height * surface->scale,
            0, 0);
    } else if (strcmp(surface->backend, "raster") == 0) {
        _raster_init(surface);
    }
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
    sb_log_debug("(sb_surface_attach) - surface: %p, backend: %s\n",
        surface, surface->backend);

    if (strcmp(surface->backend, "raster") == 0) {
        _draw_frame(surface);

        wl_surface_attach(surface->_wl_surface, surface->wl_buffer, 0, 0);
        wl_surface_damage_buffer(surface->_wl_surface, 0, 0, surface->_size.width, surface->_size.height);
        wl_surface_commit(surface->_wl_surface);
    }

    if (strcmp(surface->backend, "opengl") == 0) {
        _add_frame_callback(surface);
        _draw_frame(surface);
    }
}

void sb_surface_detach(sb_surface_t *surface)
{
    wl_surface_attach(surface->_wl_surface, NULL, 0, 0);
    wl_surface_commit(surface->_wl_surface);

    // Destroy frame callback.
    if (surface->frame_callback != NULL) {
        wl_callback_destroy(surface->frame_callback);
        surface->frame_callback = NULL;
    } else {
        sb_log_warn("sb_surface_detach() - frame_callback is already NULL\n");
    }
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
        geometry->position.x, geometry->position.y,
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
        rect->position.x, rect->position.y,
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

    if (surface->is_fractional_scale) {
        sb_log_debug("preferred_buffer_scale_handler() - "
                     "fractional scale available. ignored.\n");
        return;
    }

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

//!<======================
//!< Fractional Scale
//!<======================

static void preferred_scale_handler(void *data,
                                    struct wp_fractional_scale_v1 *wp_scale,
                                    uint32_t scale)
{
    sb_surface_t *surface = (sb_surface_t*)data;

    sb_log_debug("preferred_scale_handler - scale: %d (%f)\n",
        scale, (float)scale / (float)120);
    surface->is_fractional_scale = true;

    sb_event_t *event = sb_event_new(SB_EVENT_TARGET_TYPE_SURFACE, surface,
        SB_EVENT_TYPE_PREFERRED_SCALE);
    event->scale.scale_f = (float)scale / (float)120;

    sb_application_t *app = sb_application_instance();
    sb_application_post_event(app, event);
}
