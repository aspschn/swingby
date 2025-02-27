#ifndef _SWINGBY_SKIA_GL_CONTEXT_H
#define _SWINGBY_SKIA_GL_CONTEXT_H

#include <stdint.h>

#include <vector>

#include "skia/include/gpu/ganesh/GrDirectContext.h"
#include "skia/include/gpu/ganesh/gl/GrGLInterface.h"
#include "skia/include/core/SkSurface.h"

typedef void* EGLSurface;

typedef struct sb_egl_context_t sb_egl_context_t;

#ifdef __cplusplus
extern "C" {
#endif

typedef struct sb_skia_gl_context_t
{
    sk_sp<const GrGLInterface> gl_interface;
    sk_sp<GrDirectContext> direct_context;
    sb_egl_context_t *sb_egl_context;
    EGLSurface egl_pb_surface;
    sk_sp<SkSurface> surface;
    std::vector<uint32_t> buffer;
} sb_skia_gl_context_t;

sb_skia_gl_context_t* sb_skia_gl_context_new();

void sb_skia_gl_context_free(sb_skia_gl_context_t *gl_context);

#ifdef __cplusplus
}
#endif

#endif /* _SWINGBY_SKIA_GL_CONTEXT_H */
