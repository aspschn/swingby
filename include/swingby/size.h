#ifndef _FOUNDATION_SIZE_H
#define _FOUNDATION_SIZE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ft_size_t {
    float width;
    float height;
} ft_size_t;

/// \brief Unsigned integer version of size struct.
typedef struct ft_size_i_t {
    uint64_t width;
    uint64_t height;
} ft_size_i_t;

float ft_size_width(ft_size_t *size);

float ft_size_height(ft_size_t *size);

uint64_t ft_size_i_width(ft_size_i_t *size);

uint64_t ft_size_i_height(ft_size_i_t *size);

#ifdef __cplusplus
}
#endif

#endif /* _FOUNDATION_SIZE_H */
