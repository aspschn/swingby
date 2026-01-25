#ifndef _SWINGBY_IMAGE_IMPL_HPP
#define _SWINGBY_IMAGE_IMPL_HPP

#include <stdint.h>
#include <include/gpu/ganesh/GrBackendSurface.h>

#include "skia/include/core/SkBitmap.h"
#include "skia/include/core/SkImage.h"

#include <swingby/size.h>
#include <swingby/image.h>

class __attribute__((visibility("hidden"))) SbImageImpl
{
public:
    SbImageImpl() = delete;

    SbImageImpl(const uint8_t *data, uint64_t len);

    ~SbImageImpl();

    const sb_size_i_t& size() const;

    SkBitmap& sk_bitmap();

    GrBackendTexture& texture();

    void set_texture(GrBackendTexture& texture);

    sk_sp<SkImage> sk_image() const;

    void set_sk_image(sk_sp<SkImage> sk_image);

private:
    sb_size_i_t _size;
    SkBitmap _bitmap;
    GrBackendTexture _texture;
    sk_sp<SkImage> _image;
};

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus



#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* _SWINGBY_IMAGE_IMPL_HPP */

