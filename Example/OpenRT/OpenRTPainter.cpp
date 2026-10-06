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
#include "OpenRTPainter.h"
#include "OpenRTDevice.h"

class OpenRTDevice;

struct OpenRTPainterPrivate : UIPrivate
{
	UIPen Pen;
	UIFont Font;
	UIBrush Brush;
	UIRect Scissor, Viewport;
	bool EnableClip = false;
	UIImageRaw Target;
};
#define PRIVATE() ((OpenRTPainterPrivate*) m_Private)
#define CONTEXT() (PRIVATE()->Context)

OpenRTPainter::OpenRTPainter(UICanvasRaw canvas, int width, int height)
    :
    m_Canvas(canvas)
{
	m_Private = new OpenRTPainterPrivate;
}

OpenRTPainter::~OpenRTPainter()
{
	delete m_Private; m_Private = nullptr;
}

UICanvasRaw OpenRTPainter::getCanvas() const
{
    return m_Canvas;
}

UIImageRaw OpenRTPainter::getTarget() const
{
    return PRIVATE()->Target;
}

void OpenRTPainter::setTarget(UIImageRaw value)
{
	PRIVATE()->Target = value;
}

UIRect OpenRTPainter::boundingRect(float x, float y, float width, float height, UIString const& text, float cursor, UIRectRaw cursorRect)
{
	auto& font = PRIVATE()->Font;
	return {};
}

UIRect OpenRTPainter::boundingRect(float x, float y, float width, float height, UIString const& text, float posX, float posY, int* cursor, UIRectRaw cursorRect)
{
	auto& font = PRIVATE()->Font;
	return {};
}

void OpenRTPainter::drawImage(float x, float y, UIImage image, float sx, float sy, float sw, float sh)
{
	if (PRIVATE()->Brush.Style != UIBrush::NoBrush)
	{
		auto& font = PRIVATE()->Font;
	}
}

void OpenRTPainter::drawLine(float x1, float y1, float x2, float y2)
{
	if (PRIVATE()->Pen.Style != UIPen::NoPen)
	{
	}
}

void OpenRTPainter::drawLines(UIListView<UILine> lines)
{
	if (PRIVATE()->Pen.Style != UIPen::NoPen)
	{
	}
}

void OpenRTPainter::drawPoint(float x, float y)
{
	drawRect(x, y, 1, 1);
}

void OpenRTPainter::drawPoints(UIListView<UIPoint> points)
{
	for (size_t i = 0; i < points.size(); ++i)
	{
		drawRect(points[i].X, points[i].Y, 1, 1);
	}
}

void OpenRTPainter::drawRect(float x, float y, float width, float height)
{
	if (PRIVATE()->Brush.Style != UIBrush::NoBrush)
	{
	}
	if (PRIVATE()->Pen.Style != UIPen::NoPen)
	{
	}
}

void OpenRTPainter::drawRects(UIListView<UIRect> rects)
{
	for (size_t i = 0; i < rects.size(); ++i)
	{
		drawRect(rects[i].X, rects[i].Y, rects[i].W, rects[i].H);
	}
}

void OpenRTPainter::drawRoundedRect(float x, float y, float width, float height, float xRadius, float yRadius)
{
	if (PRIVATE()->Brush.Style != UIBrush::NoBrush)
	{
	}
	if (PRIVATE()->Pen.Style != UIPen::NoPen)
	{
	}
}

void OpenRTPainter::drawText(float x, float y, float width, float height, const UIString& text, UIRectRaw boundingRect, float cursor, UIRectRaw cursorRect)
{
	if (PRIVATE()->Brush.Style != UIBrush::NoBrush)
	{
		auto& font = PRIVATE()->Font;
	}
}

UIPen const& OpenRTPainter::getPen() const
{
	return PRIVATE()->Pen;
}

void OpenRTPainter::setPen(const UIPen& pen)
{
	PRIVATE()->Pen = pen;
}

UIBrush const& OpenRTPainter::getBrush() const
{
	return PRIVATE()->Brush;
}

void OpenRTPainter::setBrush(const UIBrush& brush)
{
	PRIVATE()->Brush = brush;
}

UIFont const& OpenRTPainter::getFont() const
{
	return PRIVATE()->Font;
}

void OpenRTPainter::setFont(const UIFont& font)
{
	PRIVATE()->Font = font;
}

void OpenRTPainter::setClipping(bool enable)
{
}

void OpenRTPainter::setClipRect(float x, float y, float width, float height)
{
	PRIVATE()->Scissor = { x, y, width, height };
}

void OpenRTPainter::setViewport(float x, float y, float width, float height)
{
	PRIVATE()->Viewport = { x, y, width, height };
}

void OpenRTPainter::skew(float sh, float sv)
{
}

void OpenRTPainter::rotate(float angle)
{
}

void OpenRTPainter::scale(float dx, float dy)
{
}

void OpenRTPainter::translate(float dx, float dy)
{
}

#endif