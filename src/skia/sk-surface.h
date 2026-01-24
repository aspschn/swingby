#ifndef _SWINGBY_SK_SURFACE_H
#define _SWINGBY_SK_SURFACE_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef struct sb_sk_surface_t sb_sk_surface_t;

sb_sk_surface_t* sb_sk_surface_new(void *gr_direct_context,
                                   int width,
                                   int height);

void* sb_sk_surface_c_ptr(const sb_sk_surface_t *sk_surface);

void sb_sk_surface_free(sb_sk_surface_t* sk_surface);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* _SWINGBY_SK_SURFACE_H */
