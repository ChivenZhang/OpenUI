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

class SDLGPUMerger : public SDLGPURender
{
public:
    SDLGPUMerger(UICanvasRaw canvas, int width, int height);
    ~SDLGPUMerger() override;
    void render(UIRect client, UIMat4 matrix, UIImageRaw srcImg, UIImageRaw dstImg, UIComputedStyleRaw style) override;

protected:
    SDL_GPUBuffer* m_Buffer;
    SDL_GPUSampler* m_Sampler;
    SDL_GPUGraphicsPipeline* m_Pipeline;
};