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
#include "SDLGPUPainter.h"

SDLGPUPainter::SDLGPUPainter(int width, int height, UICanvasRaw canvas)
    :
    m_Canvas(canvas)
{
}

UICanvasRaw SDLGPUPainter::getCanvas() const
{
    return m_Canvas;
}

UIRect SDLGPUPainter::boundingRect(float x, float y, float width, float height, const UIString& text, float cursor, UIRectRaw cursorRect)
{
    return {};
}

UIRect SDLGPUPainter::boundingRect(float x, float y, float width, float height, const UIString& text, float posX, float posY, int* cursor, UIRectRaw cursorRect)
{
    return {};
}

void SDLGPUPainter::drawPoint(float x, float y)
{
}

void SDLGPUPainter::drawPoints(UIListView<UIPoint> points)
{
}

void SDLGPUPainter::drawLine(float x1, float y1, float x2, float y2)
{
}

void SDLGPUPainter::drawLines(UIListView<UILine> lines)
{
}

void SDLGPUPainter::drawRect(float x, float y, float width, float height)
{
    auto& geometry = m_Geometry.emplace_back();
    geometry.Client = m_ClipRect;
    auto& primitive = geometry.Primitives.emplace_back();
    primitive.Clip = {x, y, width, height};
    primitive.Points.push_back({x, y});
    primitive.Points.push_back({x, y+height});
    primitive.Points.push_back({x+width, y+height});
    primitive.Points.push_back({x, y});
    primitive.Points.push_back({x+width, y+height});
    primitive.Points.push_back({x+width, y});
    primitive.Style = nullptr;
}

void SDLGPUPainter::drawRects(UIListView<UIRect> rects)
{
}

void SDLGPUPainter::drawRoundedRect(float x, float y, float w, float h, float xRadius, float yRadius)
{
}

void SDLGPUPainter::drawImage(float x, float y, UIImage image, float sx, float sy, float sw, float sh)
{
}

void SDLGPUPainter::drawText(float x, float y, float width, float height, const UIString& text, UIRectRaw boundingRect, float cursor, UIRectRaw cursorRect)
{
}

const UIPen& SDLGPUPainter::getPen() const
{
    return {};
}

void SDLGPUPainter::setPen(const UIPen& pen)
{
}

const UIBrush& SDLGPUPainter::getBrush() const
{
    return {};
}

void SDLGPUPainter::setBrush(const UIBrush& brush)
{
}

const UIFont& SDLGPUPainter::getFont() const
{
    return {};
}

void SDLGPUPainter::setFont(const UIFont& font)
{
}

void SDLGPUPainter::setClipping(bool enable)
{
}

void SDLGPUPainter::setClipRect(float x, float y, float width, float height)
{
}

void SDLGPUPainter::setViewport(float x, float y, float width, float height)
{
}

void SDLGPUPainter::shear(float sh, float sv)
{
}

void SDLGPUPainter::rotate(float angle)
{
}

void SDLGPUPainter::scale(float dx, float dy)
{
}

void SDLGPUPainter::translate(float dx, float dy)
{
}

UIList<UIGeometry>& SDLGPUPainter::getGeometry()
{
    return m_Geometry;
}

#endif
