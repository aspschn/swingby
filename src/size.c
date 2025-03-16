#include <swingby/size.h>

#ifdef __cplusplus
extern "C" {
#endif

float sb_size_width(sb_size_t *size)
{
    return size->width;
}

float sb_size_height(sb_size_t *size)
{
    return size->height;
}

bool sb_size_equals(const sb_size_t *size, const sb_size_t *other)
{
    return size->width == other->width &&
           size->height == other->height;
}

uint64_t sb_size_i_width(sb_size_i_t *size)
{
    return size->width;
}

uint64_t sb_size_i_height(sb_size_i_t *size)
{
    return size->height;
}

#ifdef __cplusplus
}
#endif
