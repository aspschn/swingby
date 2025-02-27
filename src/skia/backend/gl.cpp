#include <stdint.h>

#if defined(SB_PLATFORM_WAYLAND)
#include <EGL/egl.h>
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>
#endif

#include "skia/include/gpu/ganesh/GrDirectContext.h"
// GrBackendTexture
#include "skia/include/gpu/ganesh/GrBackendSurface.h"
// SkSurfaces::WrapBackendTexture
#include "skia/include/gpu/ganesh/SkSurfaceGanesh.h"
// GrGLTextureInfo, GrBackendTextures::GetGLTextureInfo
#include "skia/include/gpu/ganesh/gl/GrGLBackendSurface.h"
#include "skia/include/core/SkColorSpace.h"
#include "skia/include/core/SkSurface.h"

#include <swingby/log.h>

#include "../../platform/wayland/egl-context/egl-context.h"

#include "../gl-context.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(SB_PLATFORM_WAYLAND)
void sb_skia_gl_begin(sb_skia_gl_context_t *context,
                      uint32_t width,
                      uint32_t height)
{
    // Make current for pbuffer surface.
    eglMakeCurrent(context->sb_egl_context->egl_display,
        context->egl_pb_surface, context->egl_pb_surface,
        context->sb_egl_context->egl_context);

    auto direct_context = context->direct_context;

    auto image_info = SkImageInfo::Make(width, height,
        kRGBA_8888_SkColorType,
        kPremul_SkAlphaType);

    context->surface = SkSurfaces::RenderTarget(
        direct_context.get(),
        skgpu::Budgeted::kNo,
        image_info,
        0,
        nullptr // &props
    );
}

void sb_skia_gl_end(sb_skia_gl_context_t *context)
{
    auto direct_context = context->direct_context;

    direct_context->flush(context->surface.get());

    int width = context->surface->width();
    int height = context->surface->height();
    auto image_info = SkImageInfo::Make(width, height,
        kRGBA_8888_SkColorType,
        kPremul_SkAlphaType);

    // Read pixels.
    context->surface->readPixels(image_info, context->buffer.data(), 4 * width, 0, 0);

    eglMakeCurrent(context->sb_egl_context->egl_display,
        EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
}
#else
void sb_skia_gl_begin(sb_skia_gl_context_t *context,
                      uint32_t width,
                      uint32_t height)
{
    sb_log_error("sb_skia_gl_begin - GL support is only on Wayland.\n");
}

void sb_skia_gl_end(sb_skia_gl_context_t *context)
{
    sb_log_error("sb_skia_gl_end - GL support is only on Wayland.\n");
}
#endif

#ifdef __cplusplus
}
#endif
