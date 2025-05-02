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
#include "CairoMTRender.h"

CairoMTRender::CairoMTRender(uint32_t width, uint32_t height, UIRaw<SDL3MTDevice> device)
    :
    m_Width(width),
    m_Height(height),
    m_Device(device)
{
}

CairoMTRender::~CairoMTRender()
{
}

void CairoMTRender::render(UIRect client, UIListView<UIPrimitive> data)
{
}

void CairoMTRender::uploadTexture(int32_t width, int32_t height, uint8_t *pixels)
{
}

void * CairoMTRender::createShaderModule(void *device, int32_t stage, const char *source)
{
    return nullptr;
}

#endif