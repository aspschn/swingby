#ifndef _FOUNDATION_EGL_CONTEXT_H
#define _FOUNDATION_EGL_CONTEXT_H

#include <EGL/egl.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct sb_egl_context_t {
    EGLDisplay egl_display;
    EGLConfig egl_config;
    EGLContext egl_context;
} sb_egl_context_t;

sb_egl_context_t* sb_egl_context_new();

void sb_egl_context_free(sb_egl_context_t *context);

#ifdef __cplusplus
}
#endif

#endif /* _FOUNDATION_EGL_CONTEXT_H */
