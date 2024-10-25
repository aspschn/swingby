#ifndef _SWINGBY_BACKEND_GL_H
#define _SWINGBY_BACKEND_GL_H

#include "../gl-context.h"

#ifdef __cplusplus
extern "C" {
#endif

void sb_skia_gl_begin(sb_skia_gl_context_t *context,
                      uint32_t width,
                      uint32_t height);

void sb_skia_gl_end(sb_skia_gl_context_t *context);

#ifdef __cplusplus
}
#endif

#endif /* _SWINGBY_BACKEND_GL_H */
