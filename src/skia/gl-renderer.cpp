#include "gl-renderer.h"

#include <GL/gl.h>
#include <EGL/egl.h>

#include "skia/include/core/SkSurface.h"
#include "skia/include/core/SkColorSpace.h"
#include "skia/include/gpu/ganesh/GrDirectContext.h"
#include "skia/include/gpu/ganesh/gl/GrGLInterface.h"
#include "skia/include/gpu/ganesh/GrBackendSurface.h"
// GrGLInterfaces::MakeEGL
#include "skia/include/gpu/ganesh/gl/egl/GrGLMakeEGLInterface.h"
// GrDirectContexts::MakeGL
#include "skia/include/gpu/ganesh/gl/GrGLDirectContext.h"
// GrBackendRenderTargets::MakeGL
#include "skia/include/gpu/ganesh/gl/GrGLBackendSurface.h"
// SkSurfaces::WrapBackendRenderTarget
#include "skia/include/gpu/ganesh/SkSurfaceGanesh.h"

#include <swingby/application.h>
#include <swingby/log.h>

#include "../platform/wayland/egl-context/egl-context.h"
#include "../skia/sk-surface.h"

typedef struct sb_skia_gl_renderer_t {
    sk_sp<const GrGLInterface> gl_interface;
    sk_sp<GrDirectContext> direct_context;
    const SkSurface *sk_surface;
} sb_skia_gl_renderer_t;

sk_sp<const GrGLInterface> _gl_interface = nullptr;
sk_sp<GrDirectContext> _direct_context = nullptr;

sb_skia_gl_renderer_t* sb_skia_gl_renderer_new()
{
    sb_skia_gl_renderer_t *renderer = new sb_skia_gl_renderer_t;

    // renderer->gl_interface = GrGLInterfaces::MakeEGL();
    // renderer->direct_context = GrDirectContexts::MakeGL(renderer->gl_interface);

    sb_application_t *app = sb_application_instance();
    sb_egl_context_t *egl_context = sb_application_egl_context(app);

    renderer->gl_interface = nullptr;
    renderer->direct_context = nullptr;
    renderer->sk_surface = nullptr;

    // Dummy surface.
    EGLint pbuf_attribs[] = { EGL_WIDTH, 1, EGL_HEIGHT, 1, EGL_NONE };
    EGLSurface egl_surface = eglCreatePbufferSurface(
        egl_context->egl_display, egl_context->egl_config, pbuf_attribs);

    EGLBoolean res = eglMakeCurrent(egl_context->egl_display,
        egl_surface, egl_surface, egl_context->egl_context);
    if (!res) {
        EGLint err = eglGetError();
        sb_log_warn("eglMakeCurrent failed in renderer! err: 0x%x\n", err);
    }

    if (!_gl_interface) {
        _gl_interface = GrGLInterfaces::MakeEGL();
    }
    renderer->gl_interface = _gl_interface;
    if (!renderer->direct_context) {
        // _direct_context = GrDirectContexts::MakeGL(_gl_interface);
        renderer->direct_context = GrDirectContexts::MakeGL(_gl_interface);
    }

    return renderer;
}

void* sb_skia_gl_renderer_canvas(sb_skia_gl_renderer_t *renderer)
{
    return ((SkSurface*)(renderer->sk_surface))->getCanvas();
}

void sb_skia_gl_renderer_begin(sb_skia_gl_renderer_t *renderer,
                               sb_egl_context_t *egl_context,
                               EGLSurface egl_surface,
                               sb_sk_surface_t *sk_surface,
                               int width,
                               int height)
{
    EGLBoolean res = eglMakeCurrent(egl_context->egl_display,
        egl_surface, egl_surface, egl_context->egl_context);
    if (!res) {
        EGLint err = eglGetError();
        sb_log_warn("eglMakeCurrent failed in renderer! err: 0x%x\n", err);
    }

    if (!_gl_interface) {
        _gl_interface = GrGLInterfaces::MakeEGL();
    }
    renderer->gl_interface = _gl_interface;
    if (!renderer->direct_context) {
        // _direct_context = GrDirectContexts::MakeGL(_gl_interface);
        renderer->direct_context = GrDirectContexts::MakeGL(_gl_interface);
    }

    renderer->sk_surface = (const SkSurface*)sb_sk_surface_c_ptr(sk_surface);

    sb_log_debug(" == (gl_renderer) surface: %p\n", renderer->sk_surface);
}

void sb_skia_gl_renderer_end(sb_skia_gl_renderer_t *renderer)
{
    renderer->direct_context->resetContext();
    renderer->direct_context->flush();

    renderer->sk_surface = nullptr;
}

void* sb_skia_gl_renderer_gr_direct_context(sb_skia_gl_renderer_t *renderer)
{
    return (void*)(renderer->direct_context.get());
}
