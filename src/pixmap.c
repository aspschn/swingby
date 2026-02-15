#include <swingby/pixmap.h>

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

struct sb_pixmap_t {
    const uint8_t *data;
    sb_size_i_t size;
    uint64_t stride;
    enum sb_pixel_format format;
};

sb_pixmap_t* sb_pixmap_new(const uint8_t *pixels,
                           const sb_size_i_t *size,
                           uint64_t stride,
                           enum sb_pixel_format format)
{
    sb_pixmap_t *pixmap = malloc(sizeof(sb_pixmap_t));

    pixmap->data = pixels;
    pixmap->size = *size;
    pixmap->stride = stride;
    pixmap->format = format;

    return pixmap;
}

const uint8_t* sb_pixmap_data(const sb_pixmap_t *pixmap)
{
    return pixmap->data;
}

uint64_t sb_pixmap_width(const sb_pixmap_t *pixmap)
{
    return pixmap->size.width;
}

uint64_t sb_pixmap_height(const sb_pixmap_t *pixmap)
{
    return pixmap->size.height;
}

void sb_pixmap_free(sb_pixmap_t *pixmap)
{
    free(pixmap);
}

#ifdef __cplusplus
}
#endif
