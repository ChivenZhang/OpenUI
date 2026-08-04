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

SDLGPURender::SDLGPURender(int width, int height, UICanvasRaw canvas)
    :
    m_Canvas(canvas)
{
}

UICanvasRaw SDLGPURender::getCanvas() const
{
    return m_Canvas;
}

UIString SDLGPURender::getName() const
{
    return {};
}

void SDLGPURender::render(UIRect client, UIListView<const UIPrimitive> data)
{
    auto canvas = getCanvas()->getTarget();
    auto swapchainTexture = (SDL_GPUTexture*) getCanvas()->getTarget().Data;

    // // 获取命令缓冲区 (Command Buffer)
    // SDL_GPUCommandBuffer *cmdBuf = SDL_AcquireGPUCommandBuffer(m_Device);
    // if (cmdBuf == NULL)
    // {
    //     UI_ERROR("获取 GPU Command Buffer 失败: %s", SDL_GetError());
    //     return false;
    // }
    //
    // // 设置渲染目标（设置背景清屏颜色为深天蓝色）
    // SDL_GPUColorTargetInfo colorTarget = {0};
    // colorTarget.texture = swapchainTexture;
    // colorTarget.clear_color = SDL_FColor{ 0.1f, 0.2f, 0.4f, 1.0f };
    // colorTarget.load_op = SDL_GPU_LOADOP_CLEAR;
    // colorTarget.store_op = SDL_GPU_STOREOP_STORE;
    // // 开始渲染 Pass
    // SDL_GPURenderPass *renderPass = SDL_BeginGPURenderPass(
    //     cmdBuf,
    //     &colorTarget,
    //     1,
    //     NULL
    // );
    //
    // // 在此处提交管线绘制命令 (如绘制三角形/网格)
    //
    // // 结束渲染 Pass
    // SDL_EndGPURenderPass(renderPass);
    //
    // SDL_BindGPUGraphicsPipeline(pass, pipeline);
    //
    // SDL_GPUBufferBinding vb{ .buffer = vbuf, .offset = 0 };
    // SDL_BindGPUVertexBuffers(pass, 0, &vb, 1);
    //
    // SDL_DrawGPUPrimitives(pass, 3, 1, 0, 0);  // 3顶点, 1实例
}

#endif