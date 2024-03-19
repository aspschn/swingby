#include <foundation/image.h>

#include <stdint.h>
#include <stdlib.h>

#include <foundation/log.h>

#ifdef __cplusplus
extern "C" {
#endif

struct ft_image_t {
    ft_size_i_t size;
    enum ft_image_format format;
    void *data;
};

ft_image_t* ft_image_new(const ft_size_i_t *size, enum ft_image_format format)
{
    ft_image_t *image = malloc(sizeof(ft_image_t));

    if (size->width == 0 || size->height == 0) {
        ft_log_warn("ft_image_new() - Image size is invalid.\n");
    }
    image->size = *size;
    image->format = format;

    image->data = malloc(
        sizeof(uint8_t) * (image->size.width * image->size.height));

    return image;
}

const ft_size_i_t* ft_image_size(ft_image_t *image)
{
    return &image->size;
}

enum ft_image_format ft_image_image_format(ft_image_t *image)
{
    return image->format;
}

uint8_t* ft_image_data(ft_image_t *image)
{
    return image->data;
}

void ft_image_free(ft_image_t *image)
{
    free(image->data);
    free(image);
}

#ifdef __cplusplus
}
#endif
