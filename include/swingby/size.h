#ifndef SWINGBY_SIZE_H
#define SWINGBY_SIZE_H

#include <stdbool.h>
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
    int32_t width;
    int32_t height;
} sb_size_i_t;

float sb_size_width(sb_size_t *size);

float sb_size_height(sb_size_t *size);

bool sb_size_equals(const sb_size_t *size, const sb_size_t *other);

uint64_t sb_size_i_width(sb_size_i_t *size);

uint64_t sb_size_i_height(sb_size_i_t *size);

#ifdef __cplusplus
}
#endif

#endif /* SWINGBY_SIZE_H */
