#pragma once
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
#include "SDLGPURender.h"
#include <SDL3/SDL_gpu.h>

class SDLGPUInverter : public SDLGPURender
{
public:
    SDLGPUInverter(UICanvasRaw canvas, int width, int height);
    ~SDLGPUInverter() override;
    UIString getName() const override;
    void render(UIRect client, UIMat4 matrix, UIImageRaw srcImg, UIImageRaw dstImg, UIComputedStyleRaw style) override;

protected:
    SDL_GPUBuffer* m_Buffer;
    SDL_GPUSampler* m_Sampler;
    SDL_GPUGraphicsPipeline* m_Pipeline;
};