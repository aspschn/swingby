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

#ifdef __cplusplus
}
#endif

#endif /* _FOUNDATION_SIZE_H */
