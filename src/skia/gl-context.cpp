#include "gl-context.h"

#if defined(SB_PLATFORM_WAYLAND)
#include <EGL/egl.h>
#endif

// GrGLInterfaces::MakeEGL
#include "skia/include/gpu/ganesh/gl/egl/GrGLMakeEGLInterface.h"
// GrDirectContexts::MakeGL
#include "skia/include/gpu/ganesh/gl/GrGLDirectContext.h"

#include <swingby/log.h>

#include "../platform/wayland/egl-context/egl-context.h"

#if defined(SB_PLATFORM_WAYLAND)
sb_skia_gl_context_t* sb_skia_gl_context_new()
{
    sb_skia_gl_context_t *gl_context = new sb_skia_gl_context_t;

    // Initialize EGL.
    gl_context->sb_egl_context = sb_egl_context_new();
    // Create EGL Pbuffer surface.
    EGLint attrib_list[] = {
        EGL_WIDTH, 1,
        EGL_HEIGHT, 1,
        EGL_NONE,
    };
    gl_context->egl_pb_surface = eglCreatePbufferSurface(
        gl_context->sb_egl_context->egl_display,
        gl_context->sb_egl_context->egl_config,
        attrib_list
    );
    eglMakeCurrent(gl_context->sb_egl_context->egl_display,
        gl_context->egl_pb_surface,
        gl_context->egl_pb_surface,
        gl_context->sb_egl_context->egl_context
    );
    //
    gl_context->gl_interface = GrGLInterfaces::MakeEGL();
    gl_context->direct_context = GrDirectContexts::MakeGL(gl_context->gl_interface);

    gl_context->surface = nullptr;

    return gl_context;
}
#else
sb_skia_gl_context_t* sb_skia_gl_context_new()
{
    sb_log_error("sb_skia_gl_context_new - GL support is only on Wayland.\n");

    return NULL;
}
#endif
