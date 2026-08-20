#include <swingby/image.h>

#include <skia/include/core/SkBitmap.h>
#include <skia/include/core/SkPixmap.h>
#include <skia/include/core/SkImage.h>
#include <skia/include/codec/SkCodec.h>
#include <skia/include/gpu/ganesh/GrDirectContext.h>
#include <skia/include/gpu/ganesh/GrBackendSurface.h>
// SkImages::BorrowTextureFrom
#include <skia/include/gpu/ganesh/SkImageGanesh.h>

#include <swingby/size.h>
#include <swingby/log.h>
#include <swingby/pixmap.h>

#include "gl-renderer.h"
#include "renderer.h"
#include "gl-renderer.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

struct sb_image_t {
    sb_size_i_t size;
    enum sb_image_format format;
    enum sb_image_backing_type backing;
    GrBackendTexture texture;
    sk_sp<SkImage> sk_image;
};

sb_image_t* sb_image_new_from_data(const uint8_t *data, uint64_t len)
{
    sb_image_t *image = new sb_image_t;

    sk_sp<SkData> sk_data = SkData::MakeWithoutCopy(data, len);

    std::unique_ptr<SkCodec> codec = SkCodec::MakeFromData(sk_data);
    if (!codec) {
        sb_log_warn("sb_image_new_from_data - MakeFromData failed.\n");
        return image;
    }
    SkImageInfo info = codec->getInfo();
    sb_log_debug("sb_image_new_from_data - %d\n", info.colorType());

    SkBitmap bitmap;
    if (bitmap.tryAllocPixels(info) != true) {
        sb_log_warn("sb_image_new_from_data - tryAllocPixels failed.\n");
        return image;
    }

    auto res = codec->getPixels(info,
        bitmap.getPixels(), bitmap.rowBytes());
    if (res != SkCodec::kSuccess) {
        sb_log_warn("sb_image_new_from_data - Failed to decode the data.\n");
        return image;
    }

    image->size.width = bitmap.width();
    image->size.height = bitmap.height();

    image->sk_image = bitmap.asImage();

    image->backing = SB_IMAGE_BACKING_TYPE_PIXELS;

    return image;
}

sb_image_t* sb_image_new_from_pixmap(sb_pixmap_t *pixmap)
{
    sb_image_t *image = new sb_image_t;

    auto width = sb_pixmap_width(pixmap);
    auto height = sb_pixmap_height(pixmap);

    SkImageInfo info = SkImageInfo::Make(
        width,
        height,
        kRGBA_8888_SkColorType, // TODO: Conditional value.
        kUnpremul_SkAlphaType
    );

    image->format = SB_IMAGE_FORMAT_RGBA32; // TODO: Conditional value.

    SkPixmap sk_pixmap = SkPixmap(info, sb_pixmap_data(pixmap), width * 4);

    image->size.width = width;
    image->size.height = height;

    image->sk_image = SkImages::RasterFromPixmapCopy(sk_pixmap);

    image->backing = SB_IMAGE_BACKING_TYPE_PIXELS;

    return image;
}

sb_size_i_t sb_image_size(const sb_image_t *image)
{
    return image->size;
}

enum sb_image_backing_type sb_image_backing_type(const sb_image_t *image)
{
    return image->backing;
}

bool sb_image_upload_texture(sb_image_t *image)
{
    if (image->sk_image->isTextureBacked()) {
        return true;
    }

    SkPixmap pixmap;
    bool ok = image->sk_image->peekPixels(&pixmap);
    if (!ok) {
        sb_log_warn("sb_image_upload_texture - "
                    "peekPixels failed!\n");
        return false;
    }

    GrDirectContext *direct_context =
        static_cast<GrDirectContext*>(sb_skia_gl_direct_context());

    if (!direct_context) {
        sb_log_warn("sb_image_upload_texture - "
                    "Direct context is null.\n");
        return false;
    }

    GrBackendTexture texture = direct_context->createBackendTexture(
        pixmap,
        skgpu::Renderable::kNo,
        skgpu::Protected::kNo
    );
    if (texture.isValid() == false) {
        sb_log_warn("sb_image_upload_texture - "
                    "Texture is invalid!\n");
    }
    image->texture = texture;

    sk_sp<SkImage> sk_image = SkImages::BorrowTextureFrom(
        direct_context,
        texture,
        kTopLeft_GrSurfaceOrigin,
        kRGBA_8888_SkColorType,
        kPremul_SkAlphaType,
        nullptr
    );

    image->sk_image = sk_image;

    image->backing = SB_IMAGE_BACKING_TYPE_TEXTURE;

    return true;
}

enum sb_image_format sb_image_format(const sb_image_t *image)
{
    return image->format;
}

uint8_t* sb_image_data(sb_image_t *image)
{
    return nullptr;
}

void sb_image_fill(sb_image_t *image, const sb_color_t *color)
{
    //
}

void sb_image_draw_image(sb_image_t *image,
                         const sb_image_t *src,
                         const sb_point_i_t *pos,
                         enum sb_blend_mode blend_mode)
{
    //
}

void sb_image_free(sb_image_t *image)
{
    delete image;
}


void* sb_image_sk_image(const sb_image_t *image)
{
    return (void*)&image->sk_image;
}

#ifdef __cplusplus
}
#endif // __cplusplus
