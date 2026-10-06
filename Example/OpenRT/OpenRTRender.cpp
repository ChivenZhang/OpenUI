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
#include <SDL3/SDL_gpu.h>
#include <OpenUI/UICanvas.h>

#include "OpenRTDevice.h"

OpenRTRender::OpenRTRender(UICanvasRaw canvas, int width, int height)
    :
    m_Canvas(canvas)
{
}

OpenRTRender::~OpenRTRender()
{
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
    return {};
}

void OpenRTRender::delImage(UIImage value)
{
    m_Targets.emplace_back(value);
}

void OpenRTRender::render(UIRect client, UIMat4 matrix, UIImageRaw srcImg, UIImageRaw dstImg, UIComputedStyleRaw style)
{
}

#endif
