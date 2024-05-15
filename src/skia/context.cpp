#include "context.h"

#include <vector>

#include "./gl-context.h"
#include "./raster-context.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct sb_skia_gl_context_t sb_skia_gl_context_t;
typedef struct sb_skia_raster_context_t sb_skia_raster_context_t;

struct sb_skia_context_t
{
    enum sb_skia_backend backend;
    sb_skia_gl_context_t *gl_context;
    sb_skia_raster_context_t *raster_context;
};

sb_skia_context_t* sb_skia_context_new(enum sb_skia_backend backend)
{
    sb_skia_context_t *context = new sb_skia_context_t;

    context->backend = backend;

    context->raster_context = nullptr;
    context->gl_context = nullptr;

    if (backend == SB_SKIA_BACKEND_RASTER) {
        context->raster_context = sb_skia_raster_context_new();
    } else if (backend == SB_SKIA_BACKEND_GL) {
        context->gl_context = sb_skia_gl_context_new();
    }

    return context;
}

void* sb_skia_context_buffer(sb_skia_context_t *context)
{
    if (context->backend == SB_SKIA_BACKEND_RASTER) {
        return static_cast<void*>(context->raster_context->buffer.data());
    } else if (context->backend == SB_SKIA_BACKEND_GL) {
        return static_cast<void*>(context->gl_context->buffer.data());
    }

    return nullptr;
}

void sb_skia_context_set_buffer_size(sb_skia_context_t *context,
                                     uint32_t width,
                                     uint32_t height)
{
    auto total = width * height;
}

#ifdef __cplusplus
}
#endif
