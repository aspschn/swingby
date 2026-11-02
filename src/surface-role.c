#include <swingby/surface-role.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

const sb_surface_role_header_t* sb_surface_role_header(void *role)
{
    return (const sb_surface_role_header_t*)role;
}

#ifdef __cplusplus
}
#endif // __cplusplus
