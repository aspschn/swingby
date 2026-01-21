#include "image.h"

#include <skia/include/core/SkImage.h>

#ifdef __cplusplus
extern "C" {
#endif

struct sb_skia_image_t {
    sk_sp<SkImage> sk_image;
    int width;
    int height;
};

#ifdef __cplusplus
}
#endif // __cplusplus
