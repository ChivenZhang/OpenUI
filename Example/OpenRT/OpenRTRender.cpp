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
#ifdef OPENUI_ENABLE_OPENRT
#include "OpenRTRender.h"
#include <OpenRT.h>
#include <OpenUI/UICanvas.h>
#include "OpenRTX.h"

OpenRTRender::OpenRTRender(UICanvasRaw canvas)
    :
    m_Canvas(canvas)
{
}

OpenRTRender::~OpenRTRender()
{
    for (auto& texture : m_Textures)
    {
        rt_destroy_texture(texture.second);
    }
    m_Textures.clear();
    m_Targets.clear();
}

UICanvasRaw OpenRTRender::getCanvas() const
{
    return m_Canvas;
}

UIString OpenRTRender::getName() const
{
    return {};
}

UIImage OpenRTRender::newImage(uint32_t width, uint32_t height)
{
    if (width == 0 || height == 0) return {};

    while (!m_Targets.empty() && (m_Targets.back().Width != width || m_Targets.back().Height != height))
    {
        auto& texture = *(rt_texture_t*)m_Targets.back().Texture;
        rt_destroy_texture(texture);
        m_Textures.erase(texture.handle);
        m_Targets.pop_back();
    }

    UIImage image;
    if (m_Targets.empty())
    {
        rt_texture_t texture = rt_create_texture({
            .width = width,
            .height = height,
            .target = RT_TEXTURE_2D,
            .format = RT_TEXTURE_RGBA8UNORM,
            .usage = RT_TEXTURE_USAGE_COPY_SRC | RT_TEXTURE_USAGE_COPY_DST | RT_TEXTURE_USAGE_TEXTURE_BINDING | RT_TEXTURE_USAGE_RENDER_ATTACHMENT,
            .min_filter = RT_LINEAR,
            .mag_filter = RT_LINEAR,
            .address_u = RT_CLAMP_TO_BORDER,
            .address_v = RT_CLAMP_TO_BORDER,
            .address_w = RT_CLAMP_TO_BORDER,
            .mipmaps = 1,
            .border = {0.0f, 0.0f, 0.0f, 0.0f},
        });
        auto result = m_Textures.emplace(texture.handle, texture);

        image = UIImage
        {
            .Width = width,
            .Height = height,
            .Stride = width * 4,
            .Channel = 4,
            .Texture = (uint64_t)(uintptr_t)&result.first->second,
            .Format = UIImage::GPUByte,
        };
    }
    else
    {
        image = m_Targets.back();
        m_Targets.pop_back();
    }
    return image;
}

void OpenRTRender::delImage(UIImage value)
{
    if (value.Texture == 0 || value.Format != UIImage::GPUByte) return;
    m_Targets.emplace_back(value);

    auto& texture = *(rt_texture_t*)value.Texture;
    rt_pass_render_t pass{.colors = {{.texture_view = texture.default_view, .clear = true,}},};
    rt_begin_render(pass);
    rt_end_render(pass);
}

void OpenRTRender::render(UIRect client, UIMat4 matrix, UIImageRaw srcImg, UIImageRaw dstImg, UIComputedStyleRaw style)
{
    if (srcImg == nullptr || dstImg == nullptr) return;
    if (srcImg->Texture == 0 || dstImg->Texture == 0 || srcImg->Texture == dstImg->Texture) return;
    if (srcImg->Format != UIImage::GPUByte || dstImg->Format != UIImage::GPUByte) return;

    auto* src = (rt_texture_t*)srcImg->Texture;
    auto* dst = (rt_texture_t*)dstImg->Texture;
    uint32_t width = std::min(src->width, dst->width);
    uint32_t height = std::min(src->height, dst->height);
    if (width == 0 || height == 0) return;

    constexpr auto VS = R"(
        #version 460
        layout(location = 0) in vec3 in_vertex;
        layout(location = 1) in vec3 in_normal;
        layout(location = 2) in vec2 in_uv;
        layout(location = 0) out vec3 vertex;
        layout(location = 1) out vec3 normal;
        layout(location = 2) out vec2 uv;

        void main()
        {
            vertex = in_vertex;
            normal = in_normal;
            uv = in_uv;
            gl_Position = vec4(in_vertex, 1.0);
        }
    )";
    constexpr auto FS = R"(
        #version 460
        layout(location = 0) in vec3 vertex;
        layout(location = 1) in vec3 normal;
        layout(location = 2) in vec2 uv;
        layout(location = 0) out vec4 final;

        layout(binding = 0) uniform sampler2D texture0;

        void main()
        {
            final = texture(texture0, uv);
        }
    )";
    static auto module = rt_create_module_render({
        .vshader = {VS}, .fshader = {FS},
        .colors = {{.format = RT_TEXTURE_RGBA8UNORM,}},
        .vertex = {rt_vertex_vertex, {}, rt_vertex_uv,},
    });
    rt_pass_render_t pass{.colors = {{.texture_view = dst->default_view, .clear = false,}},};
    rt_begin_render(pass);
    rt_bind_module_render(module);
    rt_bind_texture_view(src->default_view, {.binding = 0,});
    static auto screen = rt_create_mesh_screen();
    rt_draw_mesh(screen);
    rt_end_render(pass);
}

#endif
