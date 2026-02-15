#ifndef _SWINGBY_SKIA_GL_RENDERER_H
#define _SWINGBY_SKIA_GL_RENDERER_H

#include <EGL/egl.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct sb_egl_context_t sb_egl_context_t;

typedef struct sb_skia_gl_renderer_t sb_skia_gl_renderer_t;

typedef struct SbImageImpl SbImageImpl;

sb_skia_gl_renderer_t* sb_skia_gl_renderer_new();

void* sb_skia_gl_renderer_canvas(sb_skia_gl_renderer_t *renderer);

void sb_skia_gl_renderer_make_image_texture(sb_skia_gl_renderer_t *renderer,
                                            SbImageImpl *image_impl);

void sb_skia_gl_renderer_begin(sb_skia_gl_renderer_t *renderer,
                               sb_egl_context_t *egl_context,
                               EGLSurface egl_surface,
                               int width,
                               int height);

void sb_skia_gl_renderer_end(sb_skia_gl_renderer_t *renderer);

void sb_skia_gl_renderer_free(sb_skia_gl_renderer_t *renderer);

#ifdef __cplusplus
}
#endif

#endif /* _SWINGBY_SKIA_GL_RENDERER_H */
