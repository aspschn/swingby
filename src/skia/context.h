#ifndef _SWINGBY_SKIA_CONTEXT_H
#define _SWINGBY_SKIA_CONTEXT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct sb_egl_context_t sb_egl_context_t;

enum sb_skia_backend {
    SB_SKIA_BACKEND_RASTER = 0,
    SB_SKIA_BACKEND_GL = 1,
};

typedef struct sb_skia_context_t sb_skia_context_t;

sb_skia_context_t* sb_skia_context_new(enum sb_skia_backend backend);

void* sb_skia_context_gl_context(sb_skia_context_t *context);

void* sb_skia_context_raster_context(sb_skia_context_t *context);

/// \brief Return the pixel buffer.
void* sb_skia_context_buffer(sb_skia_context_t *context);

void sb_skia_context_set_buffer_size(sb_skia_context_t *context,
                                     uint32_t width,
                                     uint32_t height);

void sb_skia_context_begin(sb_skia_context_t *context,
                           uint32_t width,
                           uint32_t height);

void sb_skia_context_end(sb_skia_context_t *context);

#ifdef __cplusplus
}
#endif

#endif /* _SWINGBY_SKIA_CONTEXT_H */
