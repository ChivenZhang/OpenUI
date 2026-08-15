/*=================================================
* Copyright © 2020-2026 ChivenZhang.
* All Rights Reserved.
* =====================Note=========================
*
*
* ====================History=======================
* Created by chivenzhang@gmail.com.
*
* =================================================*/
#ifdef OPENUI_ENABLE_SDLGPU
#include "SDLGPURender.h"
#include <SDL3/SDL_gpu.h>
#include <OpenUI/UICanvas.h>

#include "SDLGPUDevice.h"

SDLGPURender::SDLGPURender(UICanvasRaw canvas, int width, int height)
    :
    m_Canvas(canvas)
{
}

SDLGPURender::~SDLGPURender()
{
    auto device = UICast<SDLGPUDevice>(getCanvas()->getDevice())->getDevice();
    SDL_WaitForGPUIdle(device);

    while (m_Targets.size())
    {
        SDL_ReleaseGPUTexture(device, (SDL_GPUTexture*)m_Targets.back().Handle);
        m_Targets.pop_back();
    }
}

UICanvasRaw SDLGPURender::getCanvas() const
{
    return m_Canvas;
}

UIString SDLGPURender::getName() const
{
    return {};
}

UIImage SDLGPURender::newImage(uint32_t width, uint32_t height)
{
    auto device = UICast<SDLGPUDevice>(getCanvas()->getDevice())->getDevice();
    SDL_WaitForGPUIdle(device);

    SDL_GPUTexture* texture = nullptr;
    while (m_Targets.size() && (m_Targets.back().Width != width || m_Targets.back().Height != height))
    {
        SDL_ReleaseGPUTexture(device, (SDL_GPUTexture*)m_Targets.back().Handle);
        m_Targets.pop_back();
    }

    if (m_Targets.empty())
    {
        SDL_GPUTextureCreateInfo info
        {
            .type = SDL_GPU_TEXTURETYPE_2D,
            .format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM,
            .usage = SDL_GPU_TEXTUREUSAGE_SAMPLER | SDL_GPU_TEXTUREUSAGE_COLOR_TARGET,
            .width = width,
            .height = height,
            .layer_count_or_depth = 1,
            .num_levels = 1,
            .sample_count = SDL_GPU_SAMPLECOUNT_1,
            .props = 0
        };
        texture = SDL_CreateGPUTexture(device, &info);
    }
    else
    {
        texture = (SDL_GPUTexture*)m_Targets.back().Handle;
        m_Targets.pop_back();
    }

    return UIImage
    {
        .Width = width,
        .Height = height,
        .Stride = width * 4,
        .Channel = 4,
        .Handle = (uint64_t)texture,
        .Format = UIImage::GPUByte
    };
}

void SDLGPURender::delImage(UIImage value)
{
    m_Targets.emplace_back(value);
}

#endif