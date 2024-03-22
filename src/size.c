#include <foundation/size.h>

#ifdef __cplusplus
extern "C" {
#endif

float ft_size_width(ft_size_t *size)
{
    return size->width;
}

float ft_size_height(ft_size_t *size)
{
    return size->height;
}

uint64_t ft_size_i_width(ft_size_i_t *size)
{
    return size->width;
}

uint64_t ft_size_i_height(ft_size_i_t *size)
{
    return size->height;
}

#ifdef __cplusplus
}
#endif
