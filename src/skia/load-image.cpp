#include "load-image.h"

#include <stdlib.h>
#include <string.h>

#include <memory>

#include "skia/include/core/SkData.h"
#include "skia/include/core/SkBitmap.h"
#include "skia/include/codec/SkCodec.h"
#include "skia/include/codec/SkEncodedImageFormat.h"

#ifdef __cplusplus
extern "C" {
#endif

uint8_t* sb_skia_load_image_from_file(const char *filename,
                                      enum sb_image_file_format format,
                                      uint64_t *width,
                                      uint64_t *height)
{
    SkBitmap bitmap;

    // Load data.
    sk_sp<SkData> data = SkData::MakeFromFileName(filename);
    if (!data) {
        return NULL;
    }

    std::unique_ptr<SkCodec> codec = SkCodec::MakeFromData(data);
    if (!codec) {
        return NULL;
    }

    // Format check.
    SkEncodedImageFormat sk_format;
    switch (format) {
    case SB_IMAGE_FILE_FORMAT_PNG:
        sk_format = SkEncodedImageFormat::kPNG;
        break;
    case SB_IMAGE_FILE_FORMAT_JPEG:
        sk_format = SkEncodedImageFormat::kJPEG;
        break;
    default:
        break;
    }

    if (format != SB_IMAGE_FILE_FORMAT_AUTO) {
        if (codec->getEncodedFormat() != sk_format) {
            return NULL;
        }
    }

    SkImageInfo info = codec->getInfo().makeColorType(kRGBA_8888_SkColorType);

    auto res = bitmap.tryAllocPixels(info);
    if (res == false) {
        return NULL;
    }

    SkCodec::Result result = codec->getPixels(info, bitmap.getPixels(),
        bitmap.rowBytes());

    if (result != SkCodec::Result::kSuccess) {
        return NULL;
    }

    *width = bitmap.width();
    *height = bitmap.height();

    size_t stride = bitmap.rowBytes();
    uint8_t *ret = (uint8_t*)malloc(stride * *height);

    memcpy(ret, bitmap.getPixels(), stride * *height);

    return ret;
}

#ifdef __cplusplus
}
#endif
