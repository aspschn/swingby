#include <swingby/egl-context.h>

#include <stdlib.h>

#include <swingby/application.h>

sb_egl_context_t* sb_egl_context_new()
{
    sb_egl_context_t *context;

    context = malloc(sizeof(sb_egl_context_t));

    sb_application_t *app = sb_application_instance();

    // Get EGL display.
    context->egl_display = eglGetDisplay(
        (EGLNativeDisplayType)sb_application_wl_display(app));

    // Config and context attributes.
    EGLint config_attribs[] = {
        EGL_SURFACE_TYPE,
        EGL_WINDOW_BIT,
        EGL_RED_SIZE,
        8,
        EGL_GREEN_SIZE,
        8,
        EGL_BLUE_SIZE,
        8,
        EGL_ALPHA_SIZE,
        8,
        EGL_RENDERABLE_TYPE,
        EGL_OPENGL_BIT,
        EGL_NONE,
    };

    EGLint context_attribs[] = {
        EGL_CONTEXT_MAJOR_VERSION,
        4,
        EGL_CONTEXT_MINOR_VERSION,
        6,
        EGL_NONE,
    };

    eglBindAPI(EGL_OPENGL_API);

    // EGL initialize.
    EGLint major;
    EGLint minor;
    EGLBoolean result;
    result = eglInitialize(context->egl_display, &major, &minor);
    if (result == EGL_FALSE) {
        //
    }

    // Get EGL configs.
    EGLint count;
    eglGetConfigs(context->egl_display, NULL, 0, &count);

    EGLint choose_count;
    EGLConfig *configs = malloc(sizeof(EGLConfig) * count);
    eglChooseConfig(context->egl_display, config_attribs, configs, count,
        &choose_count);

    context->egl_config = configs[0];
    // Free?

    // Create EGL context.
    context->egl_context = eglCreateContext(context->egl_display,
        context->egl_config,
        EGL_NO_CONTEXT,
        context_attribs);

    return context;
}

void sb_egl_context_free(sb_egl_context_t *context)
{
    eglDestroyContext(context->egl_display, context->egl_context);
}
