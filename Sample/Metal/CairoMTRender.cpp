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
#define NS_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#define MTK_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION
#include "SDL3MTDevice.h"
#include "CairoMTRender.h"

static auto source = R"(
#include <metal_stdlib>
using namespace metal;

struct PSInput
{
    float4 Position [[position]];
	float2 TexCoord;
	uint Index;
};

struct VSInput
{
    float2 Position;
    uint Index;
};

PSInput vertex VSMain(device const VSInput* input [[buffer(0)]], uint vertexId [[vertex_id]])
{
    float2 pos = input[vertexId].Position;
    uint index = input[vertexId].Index;

	PSInput output;
	output.Position = float4(2*pos-1, 0.0, 1.0);
	output.TexCoord = float2(pos.x, 1-pos.y);
	output.Index = index;
	return output;
}

half4 fragment PSMain(PSInput input [[stage_in]], texture2d< half, access::sample > textureList [[texture(0)]])
{
    constexpr sampler s( address::repeat, filter::linear );
	return textureList.sample(s, input.TexCoord);
}
)";

CairoMTRender::CairoMTRender(uint32_t width, uint32_t height, UIRaw<SDL3MTDevice> _device)
    :
    m_Width(width),
    m_Height(height),
    m_Device(_device)
{
    auto device = m_Device->m_Device;
    NS::Error* error = nullptr;

    // 创建着色器库

    {
        auto library = device->newLibrary(NS::String::string(source, NS::UTF8StringEncoding), nullptr, &error);
        if (library == nullptr)
        {
            UI_FATAL("Failed to create shader library: %s", error->localizedDescription()->utf8String());
        }
        m_ShaderLibrary = library;
    }

    // 创建管线

    {
        auto pipelineDesc = MTL::RenderPipelineDescriptor::alloc()->init();
        pipelineDesc->setVertexFunction(createShaderModule(device, MTL::RenderStageVertex, source));
        pipelineDesc->setFragmentFunction(createShaderModule(device, MTL::RenderStageFragment, source));
        pipelineDesc->colorAttachments()->object(0)->setPixelFormat(MTL::PixelFormatRGBA8Unorm);
        auto pipeline = device->newRenderPipelineState(pipelineDesc, &error);
        if (pipeline == nullptr)
        {
            UI_FATAL("Failed to create render pipeline state: %s", error->localizedDescription()->utf8String());
        }
        m_RenderPipeline = pipeline;
    }

    // 创建顶点缓冲

    {
        auto buffer = device->newBuffer(sizeof(primitive_t) * 1000, MTL::ResourceStorageModeManaged);
        if (buffer == nullptr) UI_FATAL("Failed to create vertex buffer");
        m_VertexBuffer = buffer;
    }

    // 创建纹理视图

    {
        auto textureDesc = MTL::TextureDescriptor::alloc()->init();
        textureDesc->setWidth(m_Width);
        textureDesc->setHeight(m_Height);
        textureDesc->setTextureType(MTL::TextureType2D);
        textureDesc->setPixelFormat(MTL::PixelFormatBGRA8Unorm);
        textureDesc->setStorageMode(MTL::StorageModeManaged);
        textureDesc->setUsage(MTL::ResourceUsageSample | MTL::ResourceUsageRead);
        auto texture = device->newTexture(textureDesc);
        if (texture == nullptr) UI_FATAL("Failed to create texture");
        textureDesc->release();
        m_Texture = texture;
    }
}

CairoMTRender::~CairoMTRender()
{
    m_Texture->release(); m_Texture = nullptr;
    m_VertexBuffer->release(); m_VertexBuffer = nullptr;
    m_ShaderLibrary->release(); m_ShaderLibrary = nullptr;
    m_RenderPipeline->release(); m_RenderPipeline = nullptr;
    m_Device = nullptr;
}

void CairoMTRender::render(UIRect client, UIListView<UIPrimitive> data)
{
    auto device = m_Device->m_Device;
    auto commandEncoder = m_Device->m_RenderCommandEncoder;

    for (size_t i = 0; i < data.size(); ++i)
    {
        auto primitive = data[i].Primitive;
        auto painter = UICast<CairoUIPainter>(data[i].Painter);
        if (primitive.empty() || painter == nullptr) continue;

        auto width = painter->getWidth();
        auto height = painter->getHeight();

        if(m_Width < width || m_Height < height)
        {
            UI_ERROR("Too large image size %d %d", width, height);
            return;
        }

        m_PrimitiveList.clear();
        for (size_t k = 0; k < primitive.size(); ++k)
        {
            m_PrimitiveList.push_back({primitive[k].P0.X, primitive[k].P0.Y, 0});
            m_PrimitiveList.push_back({primitive[k].P1.X, primitive[k].P1.Y, 0});
            m_PrimitiveList.push_back({primitive[k].P2.X, primitive[k].P2.Y, 0});
        }
        auto range = NS::Range::Make(0, m_PrimitiveList.size() * sizeof(primitive_t));
        auto memory = m_VertexBuffer->contents();
        if (memory) ::memcpy(memory, m_PrimitiveList.data(), m_PrimitiveList.size() * sizeof(primitive_t));
        m_VertexBuffer->didModifyRange(range);

        commandEncoder->setRenderPipelineState(m_RenderPipeline);
        commandEncoder->setFragmentTexture(m_Texture, 0);
        commandEncoder->setVertexBuffer(m_VertexBuffer, 0, 0);

        MTL::Viewport viewport = {};
        viewport.originX = client.X;
        viewport.originY = client.Y;
        viewport.width = client.W;
        viewport.height = client.H;
        viewport.znear = 0.0f;
        viewport.zfar = 1.0f;
        commandEncoder->setViewport(viewport);

        MTL::ScissorRect scissor = {};
        scissor.x = (uint32_t)client.X;
        scissor.y = (uint32_t)client.Y;
        scissor.width = (uint32_t)client.W;
        scissor.height = (uint32_t)client.H;
        commandEncoder->setScissorRect(scissor);

        commandEncoder->drawPrimitives(MTL::PrimitiveTypeTriangle, 0, m_PrimitiveList.size(), 1, 0);
        break;
    }
}

void CairoMTRender::uploadTexture(int32_t width, int32_t height, uint8_t *pixels)
{
    auto device = m_Device->m_Device;

    if (width != m_Width || height != m_Height)
    {
        m_Texture->release();

        auto textureDesc = MTL::TextureDescriptor::alloc()->init();
        textureDesc->setWidth(m_Width = width);
        textureDesc->setHeight(m_Height = height);
        textureDesc->setTextureType(MTL::TextureType2D);
        textureDesc->setPixelFormat(MTL::PixelFormatBGRA8Unorm);
        textureDesc->setStorageMode(MTL::StorageModeManaged);
        textureDesc->setUsage(MTL::ResourceUsageSample | MTL::ResourceUsageRead);
        auto texture = device->newTexture(textureDesc);
        if (texture == nullptr) UI_FATAL("Failed to create texture");
        textureDesc->release();
        m_Texture = texture;
    }

    MTL::Region region = {};
    region.origin.x = 0;
    region.origin.y = 0;
    region.origin.z = 0;
    region.size.width = m_Width;
    region.size.height = m_Height;
    region.size.depth = 1;
    m_Texture->replaceRegion(region, 0, pixels, m_Width * 4);	// BGRA
}

MTL::Function * CairoMTRender::createShaderModule(MTL::Device* device, int32_t stage, const char *source)
{
    if (stage == MTL::RenderStageVertex) return m_ShaderLibrary->newFunction(NS::String::string("VSMain", NS::UTF8StringEncoding));
    if (stage == MTL::RenderStageFragment) return m_ShaderLibrary->newFunction(NS::String::string("PSMain", NS::UTF8StringEncoding));
    UI_FATAL("Unknown shader stage %d", stage);
}

#endif