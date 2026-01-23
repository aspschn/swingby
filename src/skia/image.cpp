#include "image.h"

#include <skia/include/core/SkImage.h>
#include <skia/include/core/SkData.h>
#include <skia/include/core/SkBitmap.h>
#include <skia/include/core/SkPixmap.h>
#include <skia/include/codec/SkCodec.h>

#include <swingby/log.h>

#ifdef __cplusplus
extern "C" {
#endif

struct sb_skia_image_t {
    SkBitmap sk_bitmap;
    sk_sp<SkImage> sk_image;
    sb_size_i_t size;
};

sb_skia_image_t* sb_skia_image_new()
{
    auto image = new sb_skia_image_t;

    image->sk_image = nullptr;
    image->size.width = 0;
    image->size.height = 0;

    return image;
}

bool sb_skia_image_is_null(const sb_skia_image_t* image)
{
    return image->sk_bitmap.empty();
}

bool sb_skia_image_load_from_data(sb_skia_image_t *image,
                                  const uint8_t *data,
                                  uint64_t len)
{
    sk_sp<SkData> encoded = SkData::MakeWithoutCopy(data, len);

    std::unique_ptr<SkCodec> codec = SkCodec::MakeFromData(encoded);
    if (!codec) {
        return false;
    }
    SkImageInfo info = codec->getInfo();

    if (image->sk_bitmap.tryAllocPixels(info) != true) {
        return false;
    }

    auto res = codec->getPixels(info,
        image->sk_bitmap.getPixels(), image->sk_bitmap.rowBytes());
    if (res != SkCodec::kSuccess) {
        sb_log_warn("sb_skia_image_load_from_data() - Failed to decode the data.\n");
        return false;
    }

    image->size.width = image->sk_bitmap.width();
    image->size.height = image->sk_bitmap.height();

    return true;
}

const sb_size_i_t* sb_skia_image_size(const sb_skia_image_t* image)
{
    return &image->size;
}

void* sb_skia_image_sk_bitmap(const sb_skia_image_t *image)
{
    return (void*)&image->sk_bitmap;
}

void sb_skia_image_free(sb_skia_image_t *image)
{
    // TODO: Release sk_sp<SkImage>.

    delete image;
}

#ifdef __cplusplus
}
#endif // __cplusplus
