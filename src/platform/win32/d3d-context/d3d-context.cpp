#include "d3d-context.h"

#include <stdint.h>
#include <stdlib.h>

#include <Windows.h>
#include <d3d11.h>
#include <dxgi1_6.h>
#include <d2d1_3.h>
#include <dcomp.h>

#include <swingby/log.h>

struct sb_d3d_global_context_t {
    ID3D11Device *d3dDevice;
    IDXGIDevice *dxgiDevice;
    IDXGIFactory2 *dxgiFactory;
    ID2D1Factory2 *d2dFactory;
    ID2D1Device1 *d2dDevice;
    IDCompositionDevice *dcompDevice;
};

struct sb_d3d_context_t {
    ID2D1DeviceContext *dc;
    IDXGISwapChain1 *swapChain;
    IDXGISurface2 *surface;
    ID2D1Bitmap1 *bitmap;
    IDCompositionTarget *target;
    IDCompositionVisual *visual;
};

sb_d3d_global_context_t* sb_d3d_global_context_new()
{
    sb_d3d_global_context_t *context = new sb_d3d_global_context_t;

    context->d3dDevice = NULL;
    context->dxgiDevice = NULL;
    context->dxgiFactory = NULL;
    context->d2dFactory = NULL;
    context->d2dDevice = NULL;
    context->dcompDevice = NULL;

    return context;
}

void sb_d3d_global_context_init(sb_d3d_global_context_t *context)
{
    HRESULT hr;

    hr = D3D11CreateDevice(nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        D3D11_CREATE_DEVICE_BGRA_SUPPORT,
        nullptr, 0,
        D3D11_SDK_VERSION,
        &context->d3dDevice,
        nullptr,
        nullptr
    );
    if (FAILED(hr)) {
        sb_log_error(
            "sb_d3d_global_context_init - Failed to create D3D device.\n");
    }

    hr = context->d3dDevice->QueryInterface(&context->dxgiDevice);
    if (FAILED(hr)) {
        sb_log_error(
            "sb_d3d_global_context_init - Failed to query interface!\n");
    }

    hr = CreateDXGIFactory2(
        DXGI_CREATE_FACTORY_DEBUG,
        __uuidof(context->dxgiFactory),
        (void**)&context->dxgiFactory
    );
    if (FAILED(hr)) {
        sb_log_error(
            "sb_d3d_global_context_init - Failed to create DXGI factory!\n");
    }

    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
        &context->d2dFactory);
    if (FAILED(hr)) {
        sb_log_error(
            "sb_d3d_global_context_init - Failed to create D2D factory!\n");
    }

    hr = context->d2dFactory->CreateDevice(
        context->dxgiDevice,
        &context->d2dDevice);
    if (FAILED(hr)) {
        sb_log_error(
            "sb_d3d_global_context_init - Failed to create D2D device!\n");
    }

    hr = DCompositionCreateDevice(context->dxgiDevice,
        __uuidof(context->dcompDevice),
        (void**)&context->dcompDevice
    );
    if (FAILED(hr)) {
        sb_log_error(
            "sb_d3d_global_context_init - Failed to create dcomp device. %08X\n",
            hr);
    } else {
        sb_log_debug(
            "sb_d3d_global_context_init - Create dcomp device done. %p\n",
            context->dcompDevice);
    }
}

sb_d3d_context_t* sb_d3d_context_new()
{
    sb_d3d_context_t *context = new sb_d3d_context_t;

    context->dc = NULL;
    context->swapChain = NULL;
    context->visual = NULL;
    context->target = NULL;

    return context;
}

void sb_d3d_context_init(sb_d3d_context_t *context,
                         sb_d3d_global_context_t *global_context,
                         HWND hwnd)
{
    HRESULT hr;

    hr = global_context->d2dDevice->CreateDeviceContext(
        D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
        &context->dc
    );
    if (FAILED(hr)) {
        sb_log_error(
            "sb_d3d_context_init - Failed to create device context. %08X\n",
            hr);
    }

    DXGI_SWAP_CHAIN_DESC1 desc = {};
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    desc.BufferCount = 2;
    desc.SampleDesc.Count = 1;
    desc.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;
    desc.Width = 100;
    desc.Height = 100;

    hr = global_context->dxgiFactory->CreateSwapChainForComposition(
        global_context->dxgiDevice,
        &desc,
        nullptr,
        &context->swapChain
    );
    if (FAILED(hr)) {
        sb_log_error(
            "sb_d3d_context_init - Failed to create swap chain %08X\n", hr);
    } else {
        sb_log_debug("sb_d3d_context_init - Swap chain created.\n");
    }

    // Retrieve the swap chain's back buffer.
    hr = context->swapChain->GetBuffer(0, __uuidof(context->surface),
        (void**)&context->surface);
    if (FAILED(hr)) {
        sb_log_error("sb_d3d_context_init - Get surface failed %08X\n", hr);
    }
    sb_log_debug("sb_d3d_context_init - Get surface done.\n");

    // Create a Direct2D bitmap that points to the swap chain surface.
    D2D1_BITMAP_PROPERTIES1 props = {};
    props.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
    props.pixelFormat.format = DXGI_FORMAT_R8G8B8A8_UNORM,
    props.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET |
        D2D1_BITMAP_OPTIONS_CANNOT_DRAW;

    context->dc->CreateBitmapFromDxgiSurface(
        context->surface,
        &props,
        &context->bitmap
    );
    sb_log_debug("sb_d3d_context_init - Bitmap creation done.\n");

    // Point the device context to the bitmap.
    context->dc->SetTarget(context->bitmap);
    sb_log_debug("sb_d3d_context_init - Set bitmap target done.\n");

    hr = global_context->dcompDevice->CreateVisual(&context->visual);
    if (FAILED(hr)) {
        sb_log_error("sb_d3d_context_init - Failed to create visual. %08X\n",
            hr);
    } else {
        sb_log_debug("sb_d3d_context_init - Create visual done.\n");
    }
    hr = context->visual->SetContent(context->swapChain);
    if (FAILED(hr)) {
        sb_log_error("sb_d3d_context_init - Failed to set visual content.\n");
    }

    // Create target for hwnd.
    hr = global_context->dcompDevice->CreateTargetForHwnd(hwnd,
        TRUE, &context->target);
    if (FAILED(hr)) {
        sb_log_error("sb_d3d_context_init - Failed to create target. %08X\n",
            hr);
    } else {
        sb_log_debug("sb_d3d_context_init - Create target done.\n");
    }

    context->target->SetRoot(context->visual);
    sb_log_debug("sb_d3d_context_init - Set root done.\n");

    global_context->dcompDevice->Commit();
    sb_log_debug("sb_d3d_context_init - Commit done.\n");
}

void sb_d3d_context_release(sb_d3d_context_t *context)
{
    // Release.
    context->bitmap->Release();
    context->surface->Release();
    context->visual->Release();
    context->dc->Release();
}

void sb_d3d_context_recreate(sb_d3d_context_t *context,
                             sb_d3d_global_context_t *global_context)
{
    HRESULT hr;

    hr = global_context->d2dDevice->CreateDeviceContext(
        D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
        &context->dc
    );
    if (FAILED(hr)) {
        sb_log_error(
            "sb_d3d_context_recreate - Failed to create device context. %08X\n",
            hr);
    }

    DXGI_SWAP_CHAIN_DESC1 desc = {};
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    desc.BufferCount = 2;
    desc.SampleDesc.Count = 1;
    desc.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;
    desc.Width = 100;
    desc.Height = 100;

    if (FAILED(hr)) {
        sb_log_error(
            "sb_d3d_context_recreate - Failed to create swap chain %08X\n", hr);
    } else {
        sb_log_debug("sb_d3d_context_recreate - Swap chain created.\n");
    }

    // Retrieve the swap chain's back buffer.
    hr = context->swapChain->GetBuffer(0, __uuidof(context->surface),
        (void**)&context->surface);
    if (FAILED(hr)) {
        sb_log_error("sb_d3d_context_recreate - Get surface failed %08X\n", hr);
    }
    sb_log_debug("sb_d3d_context_recreate - Get surface done.\n");

    // Create a Direct2D bitmap that points to the swap chain surface.
    D2D1_BITMAP_PROPERTIES1 props = {};
    props.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
    props.pixelFormat.format = DXGI_FORMAT_R8G8B8A8_UNORM,
    props.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET |
        D2D1_BITMAP_OPTIONS_CANNOT_DRAW;

    context->dc->CreateBitmapFromDxgiSurface(
        context->surface,
        &props,
        &context->bitmap
    );
    sb_log_debug("sb_d3d_context_recreate - Bitmap creation done.\n");

    // Point the device context to the bitmap.
    context->dc->SetTarget(context->bitmap);
    sb_log_debug("sb_d3d_context_recreate - Set bitmap target done.\n");

    hr = global_context->dcompDevice->CreateVisual(&context->visual);
    if (FAILED(hr)) {
        sb_log_error("sb_d3d_context_recreate - Failed to create visual. %08X\n",
            hr);
    } else {
        sb_log_debug("sb_d3d_context_recreate - Create visual done.\n");
    }

    hr = context->target->SetRoot(context->visual);
    if (FAILED(hr)) {
        sb_log_error("sb_d3d_context_recreate - Failed to set root!\n");
    } else {
        sb_log_debug("sb_d3d_context_recreate - Set root done.\n");
    }
}

void sb_d3d_context_swap_chain_resize_buffer(sb_d3d_context_t *context,
                                             uint32_t width,
                                             uint32_t height)
{
    HRESULT hr;

    hr = context->swapChain->ResizeBuffers(2,
        width, height,
        DXGI_FORMAT_R8G8B8A8_UNORM,
        0
    );
    if (FAILED(hr)) {
        sb_log_error(
            "sb_d3d_context_swap_chain_resize_buffer - Failed! %08X\n", hr);
    }
}

void sb_d3d_context_bitmap_copy_from_memory(sb_d3d_context_t *context,
                                            uint32_t width,
                                            uint32_t height,
                                            void *data)
{
    D2D1_RECT_U dstRect = D2D1::RectU(0, 0, width, height);

    context->bitmap->CopyFromMemory(&dstRect, data, width * 4);
}

void sb_d3d_context_swap_chain_present(sb_d3d_context_t *context)
{
    context->swapChain->Present(1, 0);
}
