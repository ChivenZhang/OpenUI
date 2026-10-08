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
#include "SDLNanoVGPainter.h"
#include "OpenUI/UICanvas.h"
#include <nanovg_sdlgpu.h>
#include <nanovg.vert.h>
#include <nanovg.frag.h>

SDLNanoVGPainter::SDLNanoVGPainter(UICanvasRaw canvas, int width, int height, SDL_GPUDevice* device)
    :
    m_Target(nullptr),
    m_Canvas(canvas)
{
    m_Context = nvgCreateSDLGPU(device, NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG,
        NVGsgShaderBundle{
        .format = SDL_GPU_SHADERFORMAT_SPIRV,
        .vertexShader = (uint8_t*)nanovg_vert,
        .vertexShaderSize = (uint32_t)sizeof(nanovg_vert),
        .fragmentShader = (uint8_t*)nanovg_frag,
        .fragmentShaderSize = (uint32_t)sizeof(nanovg_frag),
    });

    nvgInternalParams(m_Context)->userPtr
}

SDLNanoVGPainter::~SDLNanoVGPainter()
{
    nvgDeleteSDLGPU(m_Context);
    m_Context = nullptr;
}

UICanvasRaw SDLNanoVGPainter::getCanvas() const
{
    return m_Canvas;
}

UIImageRaw SDLNanoVGPainter::getTarget() const
{
    if (m_Target)
    {
        nvgEndFrame(m_Context);
    }
    return m_Target;
}

void SDLNanoVGPainter::setTarget(UIImageRaw value)
{
    m_Target = value;
    if (m_Target)
    {
        auto width = m_Canvas->getTarget()->Width;
        auto height = m_Canvas->getTarget()->Height;
        auto density = m_Canvas->getConfig().PixelDensity;
        nvgBeginFrame(m_Context, width, height, density);
    }
}

UIRect SDLNanoVGPainter::boundingRect(float x, float y, float width, float height, const UIString& text, float cursor, UIRectRaw cursorRect)
{
    return {};
}

UIRect SDLNanoVGPainter::boundingRect(float x, float y, float width, float height, const UIString& text, float posX, float posY, int* cursor, UIRectRaw cursorRect)
{
    return {};
}

void SDLNanoVGPainter::drawPoint(float x, float y)
{
}

void SDLNanoVGPainter::drawPoints(UIListView<UIPoint> points)
{
}

void SDLNanoVGPainter::drawLine(float x1, float y1, float x2, float y2)
{
}

void SDLNanoVGPainter::drawLines(UIListView<UILine> lines)
{
}

void SDLNanoVGPainter::drawRect(float x, float y, float width, float height)
{
}

void SDLNanoVGPainter::drawRects(UIListView<UIRect> rects)
{
}

void SDLNanoVGPainter::drawRoundedRect(float x, float y, float w, float h, float xRadius, float yRadius)
{
}

void SDLNanoVGPainter::drawImage(float x, float y, UIImage image, float sx, float sy, float sw, float sh)
{
}

void SDLNanoVGPainter::drawText(float x, float y, float width, float height, const UIString& text, UIRectRaw boundingRect, float cursor, UIRectRaw cursorRect)
{
}

const UIPen& SDLNanoVGPainter::getPen() const
{
    return {};
}

void SDLNanoVGPainter::setPen(const UIPen& pen)
{
}

const UIBrush& SDLNanoVGPainter::getBrush() const
{
    return {};
}

void SDLNanoVGPainter::setBrush(const UIBrush& brush)
{
}

const UIFont& SDLNanoVGPainter::getFont() const
{
    return {};
}

void SDLNanoVGPainter::setFont(const UIFont& font)
{
}

void SDLNanoVGPainter::setClipping(bool enable)
{
}

void SDLNanoVGPainter::setClipRect(float x, float y, float width, float height)
{
}

void SDLNanoVGPainter::setViewport(float x, float y, float width, float height)
{
}

void SDLNanoVGPainter::skew(float sh, float sv)
{
}

void SDLNanoVGPainter::rotate(float angle)
{
}

void SDLNanoVGPainter::scale(float dx, float dy)
{
}

void SDLNanoVGPainter::translate(float dx, float dy)
{
}