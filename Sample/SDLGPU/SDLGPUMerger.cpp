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
#include "SDLGPUMerger.h"
#include "SDLGPUDevice.h"

SDLGPUMerger::SDLGPUMerger(UICanvasRaw canvas, int width, int height)
    :
    SDLGPURender(canvas, width, height)
{

}

void SDLGPUMerger::render(UIRect client, UIMat4 matrix, UIImageRaw srcImg, UIImageRaw dstImg, UIComputedStyleRaw style)
{
    if (srcImg == nullptr || dstImg == nullptr) return;
    auto device = UICast<SDLGPUDevice>(getCanvas()->getDevice())->getDevice();
    auto srcTexture = (SDL_GPUTexture *)srcImg->Handle;
    auto dstTexture = (SDL_GPUTexture *)dstImg->Handle;

    auto cmdBuf = SDL_AcquireGPUCommandBuffer(device);
    UIAssert(cmdBuf);

    SDL_GPUColorTargetInfo colorTarget = {0};
    colorTarget.texture = dstTexture;
    colorTarget.clear_color = SDL_FColor{ 0.1f, 0.2f, 0.4f, 1.0f };
    colorTarget.load_op = SDL_GPU_LOADOP_CLEAR;
    colorTarget.store_op = SDL_GPU_STOREOP_STORE;

    auto pass = SDL_BeginGPURenderPass(
        cmdBuf,
        &colorTarget,
        1,
        nullptr
    );

    // SDL_BindGPUGraphicsPipeline(pass, pipeline);
    //
    // SDL_GPUBufferBinding vb{ .buffer = vbuf, .offset = 0 };
    // SDL_BindGPUVertexBuffers(pass, 0, &vb, 1);
    //
    // SDL_DrawGPUPrimitives(pass, 3, 1, 0, 0);  // 3顶点, 1实例

    SDL_EndGPURenderPass(pass);

	SDL_SubmitGPUCommandBuffer(cmdBuf);
}