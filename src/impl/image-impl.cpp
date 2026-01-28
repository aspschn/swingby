#include "image-impl.hpp"

#include <skia/include/core/SkData.h>
#include <skia/include/core/SkImageInfo.h>
#include <skia/include/codec/SkCodec.h>

#include <swingby/pixmap.h>
#include <swingby/log.h>

SbImageImpl::SbImageImpl(const uint8_t *data, uint64_t len)
{
    sk_sp<SkData> sk_data = SkData::MakeWithoutCopy(data, len);

    std::unique_ptr<SkCodec> codec = SkCodec::MakeFromData(sk_data);
    if (!codec) {
        sb_log_warn("SbImageImpl::SbImageImpl - MakeFromData failed.\n");
        return;
    }
    SkImageInfo info = codec->getInfo();
    sb_log_debug("SbImageImpl::SbImageImpl - %d\n", info.colorType());

    if (_bitmap.tryAllocPixels(info) != true) {
        sb_log_warn("SbImageImpl::SbImageImpl - tryAllocPixels failed.\n");
        return;
    }

    auto res = codec->getPixels(info,
        _bitmap.getPixels(), _bitmap.rowBytes());
    if (res != SkCodec::kSuccess) {
        sb_log_warn("SbImageImpl::SbImageImpl - Failed to decode the data.\n");
        return;
    }

    _size.width = _bitmap.width();
    _size.height = _bitmap.height();
}

SbImageImpl::SbImageImpl(const sb_pixmap_t *pixmap)
{
    auto width = sb_pixmap_width(pixmap);
    auto height = sb_pixmap_height(pixmap);

    SkImageInfo info = SkImageInfo::Make(
        width,
        height,
        kRGBA_8888_SkColorType, // TODO: Conditional value.
        kUnpremul_SkAlphaType
    );

    _pixmap = SkPixmap(info, sb_pixmap_data(pixmap), width * 4);

    _size.width = width;
    _size.height = height;
}

SbImageImpl::~SbImageImpl()
{
}

const sb_size_i_t& SbImageImpl::size() const
{
    return _size;
}

SkBitmap& SbImageImpl::sk_bitmap()
{
    return _bitmap;
}

GrBackendTexture& SbImageImpl::texture()
{
    return _texture;
}

void SbImageImpl::set_texture(GrBackendTexture& texture)
{
    _texture = texture;
}

sk_sp<SkImage> SbImageImpl::sk_image() const
{
    return _image;
}

void SbImageImpl::set_sk_image(sk_sp<SkImage> sk_image)
{
    _image = sk_image;
}

SkPixmap& SbImageImpl::sk_pixmap()
{
    return _pixmap;
}


#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

SbImageImpl* sb_image_impl_new_from_data(const uint8_t *data, uint64_t len)
{
    auto impl = new SbImageImpl(data, len);

    return impl;
}

SbImageImpl* sb_image_impl_new_from_pixmap(sb_pixmap_t *pixmap)
{
    auto impl = new SbImageImpl(pixmap);

    return impl;
}

void sb_image_impl_free(SbImageImpl *impl)
{
    delete impl;
}

#ifdef __cplusplus
}
#endif // __cplusplus
