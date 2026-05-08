#ifndef _FOUNDATION_EGL_CONTEXT_H
#define _FOUNDATION_EGL_CONTEXT_H

#include <EGL/egl.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct sb_egl_t {
    EGLDisplay egl_display;
    EGLConfig egl_config;
    EGLContext egl_context;
} sb_egl_t;

typedef sb_egl_t sb_egl_context_t;

sb_egl_t* sb_egl_context_new()
    __attribute__((deprecated("use sb_egl_new() instead.")));

sb_egl_t* sb_egl_new();

void sb_egl_context_free(sb_egl_context_t *context)
    __attribute__((deprecated("use sb_egl_free() instead.")));

void sb_egl_free(sb_egl_t *egl);

#ifdef __cplusplus
}
#endif

#endif /* _FOUNDATION_EGL_CONTEXT_H */
