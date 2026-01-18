#ifndef _SWINGBY_SKIA_RASTER_RENDERER_H
#define _SWINGBY_SKIA_RASTER_RENDERER_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef struct sb_skia_raster_renderer_t sb_skia_raster_renderer_t;

sb_skia_raster_renderer_t* sb_skia_raster_renderer_new();

void* sb_skia_raster_renderer_canvas(sb_skia_raster_renderer_t *renderer);

void* sb_skia_raster_renderer_buffer(sb_skia_raster_renderer_t *renderer);

void sb_skia_raster_renderer_begin(sb_skia_raster_renderer_t *renderer,
                                   int width,
                                   int height);

void sb_skia_raster_renderer_end(sb_skia_raster_renderer_t *renderer);

void sb_skia_raster_renderer_free(sb_skia_raster_renderer_t *renderer);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* _SWINGBY_SKIA_RASTER_RENDERER_H */
