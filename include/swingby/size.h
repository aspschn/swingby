#ifndef _FOUNDATION_SIZE_H
#define _FOUNDATION_SIZE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct sb_size_t {
    float width;
    float height;
} sb_size_t;

/// \brief Unsigned integer version of size struct.
typedef struct sb_size_i_t {
    uint64_t width;
    uint64_t height;
} sb_size_i_t;

float sb_size_width(sb_size_t *size);

float sb_size_height(sb_size_t *size);

uint64_t sb_size_i_width(sb_size_i_t *size);

uint64_t sb_size_i_height(sb_size_i_t *size);

#ifdef __cplusplus
}
#endif

#endif /* _FOUNDATION_SIZE_H */
