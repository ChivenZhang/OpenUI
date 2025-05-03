#pragma once
/*=================================================
* Copyright @ 2020-2025 ChivenZhang.
* All Rights Reserved.
* =====================Note=========================
*
*
* ====================History=======================
* Created by ChivenZhang at 2025/03/30 15:07:17.
*
* =================================================*/
#ifdef OPENUI_ENABLE_METAL
#include <OpenUI/UIRender.h>
#include <Metal/Metal.hpp>
class SDL3MTDevice;

class CairoMTRender : public UIRender
{
public:
    struct alignas(16) primitive_t
    {
        float X = 0, Y = 0;
        uint32_t Index = 0;
    };

public:
    CairoMTRender(uint32_t width, uint32_t height, UIRaw<SDL3MTDevice> device);
    ~CairoMTRender() override;
    void render(UIRect client, UIListView<UIPrimitive> data) override;
    void uploadTexture(int32_t width, int32_t height, uint8_t* pixels);

protected:
    MTL::Function* createShaderModule(MTL::Device* device, int32_t stage, const char* source);

protected:
    UIRaw<SDL3MTDevice> m_Device;
    MTL::RenderPipelineState* m_RenderPipeline;
    MTL::Library* m_ShaderLibrary;
    MTL::Buffer* m_VertexBuffer;
    MTL::Buffer* m_StageBuffer;
    MTL::Texture* m_Texture;
    uint32_t m_Width, m_Height;
    UIList<primitive_t> m_PrimitiveList;
};

#endif