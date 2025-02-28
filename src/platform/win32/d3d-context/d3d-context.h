#ifndef _SWINGBY_PLATFORM_WIN32_D3D_CONTEXT_D3D_CONTEXT_H
#define _SWINGBY_PLATFORM_WIN32_D3D_CONTEXT_D3D_CONTEXT_H

#include <stdint.h>

struct HWND__;
typedef struct HWND__* HWND;

#ifdef __cplusplus
extern "C" {
#endif

/// \brief A global D3D context is for application.
typedef struct sb_d3d_global_context_t sb_d3d_global_context_t;
/// \brief Context per surface.
typedef struct sb_d3d_context_t sb_d3d_context_t;

sb_d3d_global_context_t* sb_d3d_global_context_new();

void sb_d3d_global_context_init(sb_d3d_global_context_t *context);

sb_d3d_context_t* sb_d3d_context_new();

void sb_d3d_context_init(sb_d3d_context_t *context,
                         sb_d3d_global_context_t *global_context,
                         HWND hwnd);

void sb_d3d_context_swap_chain_resize_buffer(sb_d3d_context_t *context,
                                             uint32_t width,
                                             uint32_t height);

void sb_d3d_context_bitmap_copy_from_memory(sb_d3d_context_t *context,
                                            uint32_t width,
                                            uint32_t height,
                                            void *data);

void sb_d3d_context_swap_chain_present(sb_d3d_context_t *context);

#ifdef __cplusplus
}
#endif

#endif /* _SWINGBY_PLATFORM_WIN32_D3D_CONTEXT_D3D_CONTEXT_H */
