#include <stdint.h>

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>

#include "skia/include/gpu/GrDirectContext.h"
// GrBackendTexture
#include "skia/include/gpu/GrBackendSurface.h"
// SkSurfaces::WrapBackendTexture
#include "skia/include/gpu/ganesh/SkSurfaceGanesh.h"
// GrGLTextureInfo, GrBackendTextures::GetGLTextureInfo
#include "skia/include/gpu/ganesh/gl/GrGLBackendSurface.h"
#include "skia/include/core/SkSurface.h"

#include "../skia/gl-context.h"

#ifdef __cplusplus
extern "C" {
#endif

void sb_skia_gl_begin(sb_skia_gl_context_t *context,
                      uint32_t width,
                      uint32_t height)
{
    auto direct_context = context->direct_context;

    GrBackendTexture backendTexture = direct_context->createBackendTexture(
        width, height,
        kRGBA_8888_SkColorType,
        SkColors::kGreen, // TODO: Change to transparent after testing.
        skgpu::Mipmapped::kYes,
        GrRenderable::kYes
    );

    context->surface = SkSurfaces::WrapBackendTexture(
        direct_context.get(),
        backendTexture,
        GrSurfaceOrigin::kBottomLeft_GrSurfaceOrigin,
        0, // sample count
        kRGBA_8888_SkColorType, // SkColorType
        nullptr, // color space
        nullptr  // surface props. LCD font.
    );
    if (context->surface.get() == nullptr) {
        fprintf(stderr, "Surface is NULL!\n");
        return;
    }

    GrGLTextureInfo glTexInfo;
    if (!GrBackendTextures::GetGLTextureInfo(backendTexture, &glTexInfo)) {
        fprintf(stderr, "Failed to get GL texture info!\n");
    }

    GLuint texture = glTexInfo.fID;
    if (texture == 0) {
        fprintf(stderr, "Texture is 0!\n");
    } else {
        fprintf(stderr, "Texture: %d\n", texture);
    }

    GLuint framebuffer;
    glGenFramebuffers(1, &framebuffer);
    if (framebuffer == 0) {
        fprintf(stderr, "Failed to create Framebuffer object!\n");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(texture, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        fprintf(stderr, "Failed to bind framebuffer!! - %d\n", status);
    }
}

void sb_skia_gl_end(sb_skia_gl_context_t *context)
{
    auto direct_context = context->direct_context;

    direct_context->flush(context->surface.get());

    // Read pixels.
    glReadPixels(0, 0,
        context->surface->width(), context->surface->height(),
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        context->buffer.data()
    );

    eglMakeCurrent(context->sb_egl_context->egl_display,
        EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

#ifdef __cplusplus
}
#endif
