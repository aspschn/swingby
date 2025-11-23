#include "renderer.h"

// #include "./raster-renderer.h"
#include "./gl-renderer.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct sb_skia_gl_renderer_t sb_skia_gl_renderer_t;
typedef struct sb_skia_raster_renderer_t sb_skia_raster_renderer_t;

struct sb_skia_renderer_t {
    enum sb_skia_backend backend;
    sb_skia_gl_renderer_t *gl_renderer;
    sb_skia_raster_renderer_t *raster_renderer;
};

sb_skia_renderer_t* sb_skia_renderer_new(enum sb_skia_backend backend)
{
    sb_skia_renderer_t *renderer = new sb_skia_renderer_t;

    renderer->backend = backend;

    renderer->raster_renderer = nullptr;
    renderer->gl_renderer = nullptr;

    if (backend == SB_SKIA_BACKEND_RASTER) {
        renderer->raster_renderer = nullptr;
    } else if (backend == SB_SKIA_BACKEND_GL) {
        renderer->gl_renderer = sb_skia_gl_renderer_new();
    }

    return renderer;
}

enum sb_skia_backend sb_skia_renderer_backend(sb_skia_renderer_t *renderer)
{
    return renderer->backend;
}

void* sb_skia_renderer_current(sb_skia_renderer_t *renderer)
{
    if (renderer->backend == SB_SKIA_BACKEND_RASTER) {
        return static_cast<void*>(renderer->raster_renderer);
    } else if (renderer->backend == SB_SKIA_BACKEND_GL) {
        return static_cast<void*>(renderer->gl_renderer);
    }

    return nullptr;
}

void sb_skia_renderer_free(sb_skia_renderer_t *renderer)
{
    // TODO: impl
}

#ifdef __cplusplus
}
#endif
