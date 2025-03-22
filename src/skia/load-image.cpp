#include "load-image.h"

#include <stdlib.h>
#include <string.h>

#include <memory>

#include <swingby/color.h>

#include "skia/include/core/SkData.h"
#include "skia/include/core/SkBitmap.h"
#include "skia/include/core/SkImage.h"
#include "skia/include/core/SkCanvas.h"
#include "skia/include/core/SkBlendMode.h"
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

void sb_skia_image_draw_image(sb_image_t *dst,
                              const sb_image_t *src,
                              const sb_point_i_t *pos,
                              enum sb_blend_mode blend_mode)
{
    SkBitmap bitmap;
    const sb_size_i_t *dst_size = sb_image_size(dst);
    SkImageInfo info = SkImageInfo::Make(dst_size->width, dst_size->height,
        kRGBA_8888_SkColorType, kUnpremul_SkAlphaType);
    bitmap.installPixels(info, sb_image_data(dst), dst_size->width * 4);

    SkCanvas canvas(bitmap);

    // SkPaint
    SkPaint paint;
    switch (blend_mode) {
    case SB_BLEND_MODE_NONE:
        paint.setBlendMode(SkBlendMode::kSrc);
        break;
    case SB_BLEND_MODE_ALPHA:
    case SB_BLEND_MODE_PREMULTIPLIED:
        paint.setBlendMode(SkBlendMode::kSrcOver);
        break;
    default:
        paint.setBlendMode(SkBlendMode::kSrcOver);
        break;
    }

    SkBitmap src_bitmap;
    const sb_size_i_t *src_size = sb_image_size(src);
    SkAlphaType src_alpha = (blend_mode == SB_BLEND_MODE_PREMULTIPLIED)
        ? kPremul_SkAlphaType
        : kUnpremul_SkAlphaType;

    SkImageInfo src_info = SkImageInfo::Make(src_size->width, src_size->height,
                                             kRGBA_8888_SkColorType, src_alpha);
    src_bitmap.installPixels(src_info, sb_image_data((sb_image_t*)src),
        src_size->width * 4);

    sk_sp<SkImage> src_image = src_bitmap.asImage();

    // Call drawImage.
    canvas.drawImage(src_image,
        SkIntToScalar((int32_t)pos->x),
        SkIntToScalar((int32_t)pos->y),
        SkSamplingOptions(),
        &paint);
}

void sb_skia_image_fill(sb_image_t *image, const sb_color_t *color)
{
    const sb_size_i_t *size = sb_image_size(image);

    SkColorType color_type = kRGBA_8888_SkColorType; // 기본값
    SkAlphaType alpha_type = kUnpremul_SkAlphaType;

    switch (sb_image_format(image)) {
    case SB_IMAGE_FORMAT_RGBA32:
        color_type = kRGBA_8888_SkColorType;
        break;
    case SB_IMAGE_FORMAT_ARGB32:
        // color_type = kARGB_8888_SkColorType;
        break;
    default:
        return;
    }

    SkImageInfo info = SkImageInfo::Make(
        size->width, size->height, color_type, alpha_type
    );

    SkBitmap bitmap;
    if (!bitmap.installPixels(info, sb_image_data(image), size->width * 4)) {
        return;
    }

    SkCanvas canvas(bitmap);

    SkColor sk_color = SkColorSetARGB(color->a, color->r, color->g, color->b);
    canvas.clear(sk_color);
}

#ifdef __cplusplus
}
#endif
