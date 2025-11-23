#ifndef _SWINGBY_SKIA_RENDERER_H
#define _SWINGBY_SKIA_RENDERER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum sb_skia_backend {
    SB_SKIA_BACKEND_RASTER = 0,
    SB_SKIA_BACKEND_GL = 1,
};

typedef struct sb_skia_renderer_t sb_skia_renderer_t;

sb_skia_renderer_t* sb_skia_renderer_new(enum sb_skia_backend backend);

enum sb_skia_backend sb_skia_renderer_backend(sb_skia_renderer_t *renderer);

/// \brief Get current backend's renderer.
void* sb_skia_renderer_current(sb_skia_renderer_t *renderer);

void sb_skia_renderer_free(sb_skia_renderer_t *renderer);

#ifdef __cplusplus
}
#endif

#endif /* _SWINGBY_SKIA_RENDERER_H */
