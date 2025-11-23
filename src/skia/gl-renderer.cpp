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

#include <swingby/log.h>

#include "../platform/wayland/egl-context/egl-context.h"

typedef struct sb_skia_gl_renderer_t {
    sk_sp<const GrGLInterface> gl_interface;
    sk_sp<GrDirectContext> direct_context;
    sk_sp<SkSurface> surface;
} sb_skia_gl_renderer_t;

sb_skia_gl_renderer_t* sb_skia_gl_renderer_new()
{
    sb_skia_gl_renderer_t *renderer = new sb_skia_gl_renderer_t;

    renderer->gl_interface = GrGLInterfaces::MakeEGL();
    renderer->direct_context = GrDirectContexts::MakeGL(renderer->gl_interface);

    return renderer;
}

void* sb_skia_gl_renderer_canvas(sb_skia_gl_renderer_t *renderer)
{
    return renderer->surface->getCanvas();
}

void sb_skia_gl_renderer_begin(sb_skia_gl_renderer_t *renderer,
                               sb_egl_context_t *egl_context,
                               EGLSurface egl_surface,
                               int width,
                               int height)
{
    eglMakeCurrent(egl_context->egl_display,
        egl_surface, egl_surface, egl_context->egl_context);

    GrGLFramebufferInfo fb_info;
    fb_info.fFBOID = 0;
    fb_info.fFormat = GL_RGBA8;

    GrBackendRenderTarget target = GrBackendRenderTargets::MakeGL(
        width, height, 0, 8, fb_info);

    renderer->surface = SkSurfaces::WrapBackendRenderTarget(
        renderer->direct_context.get(),
        target,
        kBottomLeft_GrSurfaceOrigin,
        kRGBA_8888_SkColorType,
        nullptr,
        nullptr
    );

    sb_log_debug(" == surface: %p", renderer->surface.get());
}

void sb_skia_gl_renderer_end(sb_skia_gl_renderer_t *renderer)
{
    renderer->direct_context->flush();
}
