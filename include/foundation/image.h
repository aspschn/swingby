#ifndef _FOUNDATION_IMAGE_H
#define _FOUNDATION_IMAGE_H

#include <stdint.h>

#include <foundation/size.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum ft_image_format {
    FT_IMAGE_FORMAT_RGBA32,
    FT_IMAGE_FORMAT_ARGB32,
} ft_image_format;

typedef struct ft_image_t ft_image_t;

ft_image_t* ft_image_new(const ft_size_i_t *size, ft_image_format format);

const ft_size_i_t* ft_image_size(ft_image_t *image);

ft_image_format ft_image_image_format(ft_image_t *image);

uint8_t* ft_image_data(ft_image_t *image);

void ft_image_free(ft_image_t *image);

#ifdef __cplusplus
}
#endif

#endif /* _FOUNDATION_IMAGE_H */
