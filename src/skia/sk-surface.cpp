#include "sk-surface.h"

#include <GL/gl.h>

#include "skia/include/core/SkSurface.h"
#include "skia/include/core/SkColorSpace.h"
#include "skia/include/gpu/ganesh/GrDirectContext.h"
#include "skia/include/gpu/ganesh/GrBackendSurface.h"
// GrBackendRenderTargets::MakeGL
#include "skia/include/gpu/ganesh/gl/GrGLBackendSurface.h"
// SkSurfaces::WrapBackendRenderTarget
#include "skia/include/gpu/ganesh/SkSurfaceGanesh.h"

#include <swingby/size.h>

#include "swingby/log.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

struct sb_sk_surface_t {
    sk_sp<SkSurface> surface;
    sb_size_i_t size;
};

sb_sk_surface_t* sb_sk_surface_new(void *gr_direct_context,
                                   int width,
                                   int height)
{
    auto sk_surface = new sb_sk_surface_t;

    sk_surface->size.width = width;
    sk_surface->size.height = height;

    GrGLFramebufferInfo fb_info;
    fb_info.fFBOID = 0;
    fb_info.fFormat = GL_RGBA8;

    GrBackendRenderTarget target = GrBackendRenderTargets::MakeGL(
        width, height, 0, 8, fb_info);

    sk_surface->surface = SkSurfaces::WrapBackendRenderTarget(
        (GrDirectContext*)gr_direct_context,
        target,
        kBottomLeft_GrSurfaceOrigin,
        kRGBA_8888_SkColorType,
        nullptr,
        nullptr
    );
    if (sk_surface->surface.get() == nullptr) {
        sb_log_warn("SkSurface is %p\n", sk_surface->surface.get());
    }

    return sk_surface;
}

void* sb_sk_surface_c_ptr(const sb_sk_surface_t *sk_surface)
{
    return (void*)sk_surface->surface.get();
}

void sb_sk_surface_free(sb_sk_surface_t* sk_surface)
{
    delete sk_surface;
}

#ifdef __cplusplus
}
#endif // __cplusplus

