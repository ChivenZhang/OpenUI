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

SDLGPUPainter::SDLGPUPainter(int width, int height)
{
}

UIRect SDLGPUPainter::boundingRect(float x, float y, float width, float height, const UIString& text, float cursor, UIRectRaw cursorRect)
{
    return {};
}

UIRect SDLGPUPainter::boundingRect(float x, float y, float width, float height, const UIString& text, float posX, float posY, int* cursor, UIRectRaw cursorRect)
{
    return {};
}

void SDLGPUPainter::drawArc(float x, float y, float width, float height, float startAngle, float spanAngle)
{
}

void SDLGPUPainter::drawChord(float x, float y, float width, float height, float startAngle, float spanAngle)
{
}

void SDLGPUPainter::drawEllipse(float x, float y, float width, float height)
{
}

void SDLGPUPainter::drawImage(float x, float y, UIImage image, float sx, float sy, float sw, float sh)
{
}

void SDLGPUPainter::drawLine(float x1, float y1, float x2, float y2)
{
}

void SDLGPUPainter::drawLines(UIListView<UILine> lines)
{
}

void SDLGPUPainter::drawPie(float x, float y, float width, float height, float startAngle, float spanAngle)
{
}

void SDLGPUPainter::drawPoint(float x, float y)
{
}

void SDLGPUPainter::drawPoints(UIListView<UIPoint> points)
{
}

void SDLGPUPainter::drawPolygon(UIListView<UIPoint> points)
{
}

void SDLGPUPainter::drawPolyline(UIListView<UIPoint> points)
{
}

void SDLGPUPainter::drawRect(float x, float y, float width, float height)
{
}

void SDLGPUPainter::drawRects(UIListView<UIRect> rects)
{
}

void SDLGPUPainter::drawRoundedRect(float x, float y, float w, float h, float xRadius, float yRadius)
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

void SDLGPUPainter::resize(int width, int height)
{
}

#endif
