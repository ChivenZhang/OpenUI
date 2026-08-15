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
#include <SDL3/SDL_gpu.h>
#include <merger.vert.h>
#include <merger.frag.h>

SDLGPUMerger::SDLGPUMerger(UICanvasRaw canvas, int width, int height)
    :
    SDLGPURender(canvas, width, height)
{
    auto device = UICast<SDLGPUDevice>(getCanvas()->getDevice())->getDevice();

    SDL_GPUShaderCreateInfo info1
    {
        .code_size             = sizeof(merger_vert),
        .code                  = (uint8_t*)(void*)merger_vert,
        .entrypoint            = "main",
        .format                = SDL_GPU_SHADERFORMAT_SPIRV,
        .stage                 = SDL_GPU_SHADERSTAGE_VERTEX,
        .num_samplers          = 0,
        .num_storage_textures  = 0,
        .num_storage_buffers   = 0,
        .num_uniform_buffers   = 0,
    };
    m_VShader = SDL_CreateGPUShader(device, &info1);

    SDL_GPUShaderCreateInfo info2
    {
        .code_size             = sizeof(merger_frag),
        .code                  = (uint8_t*)(void*)merger_frag,
        .entrypoint            = "main",
        .format                = SDL_GPU_SHADERFORMAT_SPIRV,
        .stage                 = SDL_GPU_SHADERSTAGE_FRAGMENT,
        .num_samplers          = 2,
        .num_storage_textures  = 0,
        .num_storage_buffers   = 0,
        .num_uniform_buffers   = 0,
    };
    m_FShader = SDL_CreateGPUShader(device, &info2);
}

SDLGPUMerger::~SDLGPUMerger()
{
    auto device = UICast<SDLGPUDevice>(getCanvas()->getDevice())->getDevice();

    SDL_ReleaseGPUShader(device, m_VShader); m_VShader = nullptr;
    SDL_ReleaseGPUShader(device, m_FShader); m_FShader = nullptr;
}

void SDLGPUMerger::render(UIRect client, UIMat4 matrix, UIImageRaw srcImg, UIImageRaw dstImg, UIComputedStyleRaw style)
{
    if (srcImg == nullptr || dstImg == nullptr) return;
    auto device = UICast<SDLGPUDevice>(getCanvas()->getDevice())->getDevice();
    auto srcTexture = (SDL_GPUTexture *)srcImg->Handle;
    auto dstTexture = (SDL_GPUTexture *)dstImg->Handle;

    auto cmdBuf = SDL_AcquireGPUCommandBuffer(device);
    UIAssert(cmdBuf);

    SDL_GPUColorTargetInfo colorTarget = {};
    colorTarget.texture = dstTexture;
    colorTarget.clear_color = SDL_FColor{ 0, 1, 1, 1 };
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
