#ifndef _FOUNDATION_EGL_CONTEXT_H
#define _FOUNDATION_EGL_CONTEXT_H

#include <EGL/egl.h>

typedef struct ft_egl_context_t {
    EGLDisplay egl_display;
    EGLConfig egl_config;
    EGLContext egl_context;
} ft_egl_context_t;

ft_egl_context_t* ft_egl_context_new();

#endif /* _FOUNDATION_EGL_CONTEXT_H */
