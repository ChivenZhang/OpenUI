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
#include "SDLGPUInverter.h"
#include "SDLGPUDevice.h"
#include <SDL3/SDL_gpu.h>
#include <invert.vert.h>
#include <invert.frag.h>

static const UIPointUV vertices[]
{
    {-1.0f, -1.0f, 0.0f, 1.0f}, // 左下
    {3.0f, -1.0f, 2.0f, 1.0f}, // 右下（超出右边界）
    {-1.0f, 3.0f, 0.0f, -1.0f}, // 左上（超出上边界）
};

SDLGPUInverter::SDLGPUInverter(UICanvasRaw canvas, int width, int height)
    :
    SDLGPURender(canvas, width, height)
{
    auto device = UICast<SDLGPUDevice>(getCanvas()->getDevice())->getDevice();

    // Create Graphics Pipeline

    SDL_GPUShaderCreateInfo vsInfo
    {
        .code_size = sizeof(invert_vert),
        .code = (uint8_t*)(void*)invert_vert,
        .entrypoint = "VSMain",
        .format = SDL_GPU_SHADERFORMAT_SPIRV,
        .stage = SDL_GPU_SHADERSTAGE_VERTEX,
        .num_samplers = 0,
        .num_storage_textures = 0,
        .num_storage_buffers = 0,
        .num_uniform_buffers = 0,
    };
    auto vshader = SDL_CreateGPUShader(device, &vsInfo);
    if (!vshader) UI_FATAL("vshader create failed: %s", SDL_GetError());

    SDL_GPUShaderCreateInfo fsInfo
    {
        .code_size = sizeof(invert_frag),
        .code = (uint8_t*)(void*)invert_frag,
        .entrypoint = "FSMain",
        .format = SDL_GPU_SHADERFORMAT_SPIRV,
        .stage = SDL_GPU_SHADERSTAGE_FRAGMENT,
        .num_samplers = 1,
        .num_storage_textures = 0,
        .num_storage_buffers = 0,
        .num_uniform_buffers = 0,
    };
    auto fshader = SDL_CreateGPUShader(device, &fsInfo);
    if (!fshader) UI_FATAL("fshader create failed: %s", SDL_GetError());

    SDL_GPUColorTargetDescription colorTarget = {};
    colorTarget.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
    colorTarget.blend_state.enable_blend = true;
    colorTarget.blend_state.color_blend_op = SDL_GPU_BLENDOP_ADD;
    colorTarget.blend_state.src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
    colorTarget.blend_state.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    colorTarget.blend_state.alpha_blend_op = SDL_GPU_BLENDOP_ADD;
    colorTarget.blend_state.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
    colorTarget.blend_state.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    colorTarget.blend_state.color_write_mask =
        SDL_GPU_COLORCOMPONENT_R | SDL_GPU_COLORCOMPONENT_G |
        SDL_GPU_COLORCOMPONENT_B | SDL_GPU_COLORCOMPONENT_A;

    SDL_GPUVertexBufferDescription vbDesc = {
        .slot = 0,
        .pitch = sizeof(UIPointUV),
        .input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX,
        .instance_step_rate = 0,
    };
    SDL_GPUVertexAttribute attrs[2] = {
        {
            .location = 0,
            .buffer_slot = 0,
            .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2,
            .offset = offsetof(UIPointUV, X),
        },
        {
            .location = 1,
            .buffer_slot = 0,
            .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2,
            .offset = offsetof(UIPointUV, U),
        },
    };
    SDL_GPUVertexInputState viState = {
        .vertex_buffer_descriptions = &vbDesc,
        .num_vertex_buffers = 1,
        .vertex_attributes = attrs,
        .num_vertex_attributes = 2,
    };

    SDL_GPURasterizerState raster = {};
    raster.fill_mode = SDL_GPU_FILLMODE_FILL;
    raster.cull_mode = SDL_GPU_CULLMODE_NONE;
    raster.front_face = SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE;

    SDL_GPUDepthStencilState ds = {};
    ds.enable_depth_test = false;
    ds.enable_depth_write = false;

    SDL_GPUGraphicsPipelineCreateInfo pipeInfo = {};
    pipeInfo.vertex_shader = vshader;
    pipeInfo.fragment_shader = fshader;
    pipeInfo.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
    pipeInfo.vertex_input_state = viState;
    pipeInfo.rasterizer_state = raster;
    pipeInfo.depth_stencil_state = ds;
    pipeInfo.target_info.num_color_targets = 1;
    pipeInfo.target_info.color_target_descriptions = &colorTarget;

    m_Pipeline = SDL_CreateGPUGraphicsPipeline(device, &pipeInfo);
    if (!m_Pipeline) UI_FATAL("pipeline create failed: %s", SDL_GetError());
    SDL_ReleaseGPUShader(device, vshader);
    SDL_ReleaseGPUShader(device, fshader);

    // Create Vertex Buffer

    SDL_GPUBufferCreateInfo bufInfo = {0};
    bufInfo.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
    bufInfo.size = sizeof(vertices);
    bufInfo.props = 0;
    m_Buffer = SDL_CreateGPUBuffer(device, &bufInfo);
    if (!m_Buffer) UI_FATAL("vertex buffer create failed: %s", SDL_GetError());

    auto cmd = SDL_AcquireGPUCommandBuffer(device);
    if (!cmd) UI_FATAL("acquire cmd buffer failed");
    SDL_GPUTransferBufferCreateInfo tbInfo
    {
        .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
        .size = sizeof(vertices),
    };
    auto transferBuf = SDL_CreateGPUTransferBuffer(device, &tbInfo);
    if (!transferBuf) UI_FATAL("transfer buffer create failed: %s", SDL_GetError());
    if (void* mapped = SDL_MapGPUTransferBuffer(device, transferBuf, false))
    {
        SDL_memcpy(mapped, vertices, sizeof(vertices));
        SDL_UnmapGPUTransferBuffer(device, transferBuf);
    }
    SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(cmd);
    SDL_GPUTransferBufferLocation srcRange
    {
        .transfer_buffer = transferBuf,
        .offset = 0
    };
    SDL_GPUBufferRegion dstRange
    {
        .buffer = m_Buffer,
        .offset = 0,
        .size = sizeof(vertices)
    };
    SDL_UploadToGPUBuffer(copyPass, &srcRange, &dstRange, false);
    SDL_EndGPUCopyPass(copyPass);
    SDL_SubmitGPUCommandBuffer(cmd);
    SDL_ReleaseGPUTransferBuffer(device, transferBuf);

    // Create Texture Sampler

    SDL_GPUSamplerCreateInfo samplerInfo
    {
        .min_filter = SDL_GPU_FILTER_LINEAR,
        .mag_filter = SDL_GPU_FILTER_LINEAR,
        .mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR,
        .address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
        .address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
        .address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
    };
    m_Sampler = SDL_CreateGPUSampler(device, &samplerInfo);
    if (!m_Sampler) UI_FATAL("sampler create failed: %s", SDL_GetError());
}

SDLGPUInverter::~SDLGPUInverter()
{
    auto device = UICast<SDLGPUDevice>(getCanvas()->getDevice())->getDevice();
    SDL_WaitForGPUIdle(device);

    SDL_ReleaseGPUBuffer(device, m_Buffer);
    m_Buffer = nullptr;
    SDL_ReleaseGPUSampler(device, m_Sampler);
    m_Sampler = nullptr;
    SDL_ReleaseGPUGraphicsPipeline(device, m_Pipeline);
    m_Pipeline = nullptr;
}

UIString SDLGPUInverter::getName() const
{
    return "invert";
}

void SDLGPUInverter::render(UIRect client, UIMat4 matrix, UIImageRaw srcImg, UIImageRaw dstImg, UIComputedStyleRaw style)
{
    if (srcImg == nullptr || dstImg == nullptr) return;
    auto device = UICast<SDLGPUDevice>(getCanvas()->getDevice())->getDevice();
    auto srcTexture = (SDL_GPUTexture*)srcImg->Handle;
    auto dstTexture = (SDL_GPUTexture*)dstImg->Handle;

    auto cmdBuf = SDL_AcquireGPUCommandBuffer(device);

    if (cmdBuf)
    {
        SDL_GPUColorTargetInfo colorTarget = {};
        colorTarget.texture = dstTexture;
        colorTarget.clear_color = SDL_FColor{0, 0, 0, 1};
        colorTarget.load_op = SDL_GPU_LOADOP_CLEAR;
        colorTarget.store_op = SDL_GPU_STOREOP_STORE;
        auto pass = SDL_BeginGPURenderPass(
            cmdBuf,
            &colorTarget,
            1,
            nullptr
        );

        SDL_BindGPUGraphicsPipeline(pass, m_Pipeline);

        SDL_GPUBufferBinding bindBuffer{.buffer = m_Buffer, .offset = 0,};
        SDL_BindGPUVertexBuffers(pass, 0, &bindBuffer, 1);

        SDL_GPUTextureSamplerBinding bindSampler = {.texture = srcTexture, .sampler = m_Sampler};
        SDL_BindGPUFragmentSamplers(pass, 0, &bindSampler, 1);

        SDL_DrawGPUPrimitives(pass, 3, 1, 0, 0);

        SDL_EndGPURenderPass(pass);
    }

    SDL_SubmitGPUCommandBuffer(cmdBuf);
}
