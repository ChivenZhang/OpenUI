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
#include "SDLGPURenderVG.h"
#include "SDLGPUDevice.h"
#include <OpenVG/VGContext.h>
#include <OpenVG/VGShape.h>
#include <OpenVG/VGText.h>
#include <OpenVG/VGPicture.h>
#include <OpenVG/VGTrueType.h>

class SDLGPUDevice;

struct SDLGPUPainterPrivate : UIPrivate
{
	VGRect Client;
	VGContextRef Context;
	UIPen Pen;
	UIFont Font;
	UIBrush Brush;
	UIRect Scissor, Viewport;
	bool EnableClip = false;
	VGShapeRef RectFillShape;
	UIImageRaw Target;
};
#define PRIVATE() ((SDLGPUPainterPrivate*) m_Private)
#define CONTEXT() (PRIVATE()->Context)

SDLGPUPainter::SDLGPUPainter(UICanvasRaw canvas, int width, int height)
    :
    m_Canvas(canvas)
{
	m_Private = new SDLGPUPainterPrivate;
	PRIVATE()->Context = VGNew<VGContext>();
	CONTEXT()->setPainter(VGNew<VGPainter>());
	CONTEXT()->setRender(VGNew<SDLGPURenderVG>(VGCast<SDLGPUDevice>(canvas->getDevice())->getDevice()));
}

SDLGPUPainter::~SDLGPUPainter()
{
	delete m_Private; m_Private = nullptr;
}

UICanvasRaw SDLGPUPainter::getCanvas() const
{
    return m_Canvas;
}

UIImageRaw SDLGPUPainter::getTarget() const
{
	auto target = PRIVATE()->Target;
	VGImage source
	{
		.Width = target->Width,
		.Height = target->Height,
		.Stride = target->Stride,
		.Channel = target->Channel,
		.Handle = target->Handle,
		.Format = (decltype(VGImage::Format))target->Format,
	};
	CONTEXT()->getRender()->setTarget(&source);
	CONTEXT()->renderElement({0, 0, (float)PRIVATE()->Target->Width, (float)PRIVATE()->Target->Height});
    return PRIVATE()->Target;
}

void SDLGPUPainter::setTarget(UIImageRaw value)
{
	PRIVATE()->Target = value;
}

UIRect SDLGPUPainter::boundingRect(float x, float y, float width, float height, UIString const& text, float cursor, UIRectRaw cursorRect)
{
	auto& font = PRIVATE()->Font;

	VGText shape;
	shape.setSize(font.Size);
	shape.setFamily(font.Family);
	shape.setSpacing(font.Spacing);
	shape.setLineWrap(font.LineWrap);
	shape.setLineSpacing(font.LineSpacing);
	auto color = getBrush().Color;
	shape.setFillColor({ color.R, color.G, color.B, color.A });
	auto scissor = PRIVATE()->Scissor;
	shape.setScissor({ scissor.X, scissor.Y, scissor.W, scissor.H });
	shape.setText(0, 0, width, height, text);

	VGRect rect = { x, y, width, height };
	VGRect _cursorRect;
	VGTrueType::Measure(&shape, rect, text, cursor, &_cursorRect, rect);
	int32_t text_width = rect.W, text_height = rect.H, baseline = 0;

	if (font.Align & UIFont::AlignLeft) { rect.X = x; }
	else if (font.Align & UIFont::AlignRight) { rect.X = x + width - text_width; }
	else if (font.Align & UIFont::AlignCenter) { rect.X = x + std::round((width - text_width) * 0.5f); }
	// else if (font.Align & UIFont::AlignJustify) { rect.X = x + baseline; }
	else { rect.X = x; }

	if (font.Align & UIFont::AlignTop) { rect.Y = y; }
	else if (font.Align & UIFont::AlignBottom) { rect.Y = y + height - text_height; }
	else if (font.Align & UIFont::AlignVCenter) { rect.Y = y + std::round((height - text_height) * 0.5f); }
	// else if (font.Align & UIFont::AlignBaseline) { rect.Y = y + baseline; }
	else { rect.Y = y; }

	if (cursorRect)
	{
		*cursorRect = { rect.X + _cursorRect.X, rect.Y + _cursorRect.Y, _cursorRect.W, _cursorRect.H };
	}

	return UIRect{ rect.X, rect.Y, rect.W, rect.H };
}

UIRect SDLGPUPainter::boundingRect(float x, float y, float width, float height, UIString const& text, float posX, float posY, int* cursor, UIRectRaw cursorRect)
{
	auto& font = PRIVATE()->Font;

	VGText shape;
	shape.setSize(font.Size);
	shape.setFamily(font.Family);
	shape.setSpacing(font.Spacing);
	shape.setLineWrap(font.LineWrap);
	shape.setLineSpacing(font.LineSpacing);
	auto color = getBrush().Color;
	shape.setFillColor({ color.R, color.G, color.B, color.A });
	auto scissor = PRIVATE()->Scissor;
	shape.setScissor({ scissor.X, scissor.Y, scissor.W, scissor.H });
	shape.setText(0, 0, width, height, text);

	VGRect rect = { x, y, width, height };
	float _cursor;
	VGRect _cursorRect;
	VGTrueType::Measure(&shape, rect, text, posX - x, posY - y, &_cursor, &_cursorRect, rect);
	int32_t text_width = rect.W, text_height = rect.H, baseline = 0;

	if (font.Align & UIFont::AlignLeft) { rect.X = x; }
	else if (font.Align & UIFont::AlignRight) { rect.X = x + width - text_width; }
	else if (font.Align & UIFont::AlignCenter) { rect.X = x + std::round((width - text_width) * 0.5f); }
	// else if (font.Align & UIFont::AlignJustify) { rect.X = x + baseline; }
	else { rect.X = x; }

	if (font.Align & UIFont::AlignTop) { rect.Y = y; }
	else if (font.Align & UIFont::AlignBottom) { rect.Y = y + height - text_height; }
	else if (font.Align & UIFont::AlignVCenter) { rect.Y = y + std::round((height - text_height) * 0.5f); }
	// else if (font.Align & UIFont::AlignBaseline) { rect.Y = y + baseline; }
	else { rect.Y = y; }

	if (cursorRect)
	{
		if (cursor) *cursor = _cursor;
		*cursorRect = { rect.X + _cursorRect.X, rect.Y + _cursorRect.Y, _cursorRect.W, _cursorRect.H };
	}

	return UIRect{ rect.X, rect.Y, rect.W, rect.H };
}

void SDLGPUPainter::drawImage(float x, float y, UIImage image, float sx, float sy, float sw, float sh)
{
	if (PRIVATE()->Brush.Style != UIBrush::NoBrush)
	{
		auto& font = PRIVATE()->Font;

		VGPicture shape;
		auto color = getBrush().Color;
		shape.setFillColor({ color.R, color.G, color.B, color.A });
		auto scissor = PRIVATE()->Scissor;
		shape.setScissor({ scissor.X, scissor.Y, scissor.W, scissor.H });

		VGImage _image;
		_image.Width = image.Width;
		_image.Height = image.Height;
		_image.Stride = image.Stride;
		_image.Pixels = image.Pixels;
		_image.Format = (decltype(VGImage::Format))image.Format;
		shape.setImage(0, 0, image.Width, image.Height, _image);

		shape.setScale(1, 1);
		shape.setRotate(0);
		shape.setTranslate(x, y);
		CONTEXT()->fillElement(&shape);
	}
}

void SDLGPUPainter::drawLine(float x1, float y1, float x2, float y2)
{
	if (PRIVATE()->Pen.Style != UIPen::NoPen)
	{
		VGShape shape;
		shape.moveTo(x1, y1);
		shape.lineTo(x2, y2);
		shape.close();
		shape.setLineWidth(getPen().Width);
		auto color = getPen().Color;
		shape.setStrokeColor({ color.R, color.G, color.B, color.A });
		auto scissor = PRIVATE()->Scissor;
		shape.setScissor({ scissor.X, scissor.Y, scissor.W, scissor.H });
		CONTEXT()->strokeElement(&shape);
	}
}

void SDLGPUPainter::drawLines(UIListView<UILine> lines)
{
	if (PRIVATE()->Pen.Style != UIPen::NoPen)
	{
		VGShape shape;
		for (size_t i = 0; i < lines.size(); ++i)
		{
			shape.moveTo(lines[i].P0.X, lines[i].P0.Y);
			shape.lineTo(lines[i].P1.X, lines[i].P1.Y);
			shape.close();
		}
		shape.setLineWidth(getPen().Width);
		auto color = getPen().Color;
		shape.setStrokeColor({ color.R, color.G, color.B, color.A });
		auto scissor = PRIVATE()->Scissor;
		shape.setScissor({ scissor.X, scissor.Y, scissor.W, scissor.H });
		CONTEXT()->strokeElement(&shape);
	}
}

void SDLGPUPainter::drawPoint(float x, float y)
{
	drawRect(x, y, 1, 1);
}

void SDLGPUPainter::drawPoints(UIListView<UIPoint> points)
{
	for (size_t i = 0; i < points.size(); ++i)
	{
		drawRect(points[i].X, points[i].Y, 1, 1);
	}
}

void SDLGPUPainter::drawRect(float x, float y, float width, float height)
{
	if (PRIVATE()->Brush.Style != UIBrush::NoBrush)
	{
		if (PRIVATE()->RectFillShape == nullptr)
		{
			PRIVATE()->RectFillShape = VGNew<VGShape>();
			PRIVATE()->RectFillShape->moveTo(0, 0);
			PRIVATE()->RectFillShape->lineTo(0, 0 + 100);
			PRIVATE()->RectFillShape->lineTo(0 + 100, 0 + 100);
			PRIVATE()->RectFillShape->lineTo(0 + 100, 0);
			PRIVATE()->RectFillShape->close();
		}
		auto& shape = *PRIVATE()->RectFillShape.get();
		shape.setRotate(0);
		shape.setTranslate({ x, y });
		shape.setScale({ width * 0.01f , height * 0.01f });
		auto color = getBrush().Color;
		shape.setFillColor({ color.R, color.G, color.B, color.A });
		auto scissor = PRIVATE()->Scissor;
		shape.setScissor({ scissor.X, scissor.Y, scissor.W, scissor.H });
		CONTEXT()->fillElement(&shape);
	}
	if (PRIVATE()->Pen.Style != UIPen::NoPen)
	{
		VGShape shape;
		shape.reset();
		shape.moveTo(x, y);
		shape.lineTo(x, y + height);
		shape.lineTo(x + width, y + height);
		shape.lineTo(x + width, y);
		shape.close();
		shape.setRotate(0);
		shape.setScale(1, 1);
		shape.setTranslate(0, 0);
		shape.setLineWidth(getPen().Width);
		auto color = getPen().Color;
		shape.setStrokeColor({ color.R, color.G, color.B, color.A });
		auto scissor = PRIVATE()->Scissor;
		shape.setScissor({ scissor.X, scissor.Y, scissor.W, scissor.H });
		CONTEXT()->strokeElement(&shape);
	}
}

void SDLGPUPainter::drawRects(UIListView<UIRect> rects)
{
	for (size_t i = 0; i < rects.size(); ++i)
	{
		drawRect(rects[i].X, rects[i].Y, rects[i].W, rects[i].H);
	}
}

void SDLGPUPainter::drawRoundedRect(float x, float y, float width, float height, float xRadius, float yRadius)
{
	if (PRIVATE()->Brush.Style != UIBrush::NoBrush)
	{
		VGShape shape;
		shape.reset();
		shape.arcTo(0 + xRadius, 0 + yRadius, xRadius, yRadius, 180, 90);
		shape.arcTo(0 + width - xRadius, 0 + yRadius, xRadius, yRadius, -90, 90);
		shape.arcTo(0 + width - xRadius, 0 + height - yRadius, xRadius, yRadius, 0, 90);
		shape.arcTo(0 + xRadius, 0 + height - yRadius, xRadius, yRadius, 90, 90);
		shape.close();
		shape.setTranslate({ x, y });
		auto color = getBrush().Color;
		shape.setFillColor({ color.R, color.G, color.B, color.A });
		auto scissor = PRIVATE()->Scissor;
		shape.setScissor({ scissor.X, scissor.Y, scissor.W, scissor.H });
		CONTEXT()->fillElement(&shape);
	}
	if (PRIVATE()->Pen.Style != UIPen::NoPen)
	{
		VGShape shape;
		shape.reset();
		shape.arcTo(0 + xRadius, 0 + yRadius, xRadius, yRadius, 180, 90);
		shape.arcTo(0 + width - xRadius, 0 + yRadius, xRadius, yRadius, -90, 90);
		shape.arcTo(0 + width - xRadius, 0 + height - yRadius, xRadius, yRadius, 0, 90);
		shape.arcTo(0 + xRadius, 0 + height - yRadius, xRadius, yRadius, 90, 90);
		shape.close();
		shape.setTranslate({ x, y });
		shape.setLineWidth(getPen().Width);
		auto color = getPen().Color;
		shape.setStrokeColor({ color.R, color.G, color.B, color.A });
		auto scissor = PRIVATE()->Scissor;
		shape.setScissor({ scissor.X, scissor.Y, scissor.W, scissor.H });
		CONTEXT()->strokeElement(&shape);
	}
}

void SDLGPUPainter::drawText(float x, float y, float width, float height, const UIString& text, UIRectRaw boundingRect, float cursor, UIRectRaw cursorRect)
{
	if (PRIVATE()->Brush.Style != UIBrush::NoBrush)
	{
		auto& font = PRIVATE()->Font;

		VGText shape;
		shape.setSize(font.Size);
		shape.setFamily(font.Family);
		shape.setSpacing(font.Spacing);
		shape.setLineWrap(font.LineWrap);
		shape.setLineSpacing(font.LineSpacing);
		auto color = getBrush().Color;
		shape.setFillColor({ color.R, color.G, color.B, color.A });
		auto scissor = PRIVATE()->Scissor;
		shape.setScissor({ scissor.X, scissor.Y, scissor.W, scissor.H });
		shape.setText(0, 0, width, height, text);

		VGRect rect = { x, y, width, height };
		VGRect _cursorRect;
		VGTrueType::Measure(&shape, rect, text, cursor, &_cursorRect, rect);
		int32_t text_width = rect.W, text_height = rect.H, baseline = 0;

		if (font.Align & UIFont::AlignLeft) { rect.X = x; }
		else if (font.Align & UIFont::AlignRight) { rect.X = x + width - text_width; }
		else if (font.Align & UIFont::AlignCenter) { rect.X = x + std::round((width - text_width) * 0.5f); }
		// else if (font.Align & UIFont::AlignJustify) { rect.X = x + baseline; }
		else { rect.X = x; }

		if (font.Align & UIFont::AlignTop) { rect.Y = y; }
		else if (font.Align & UIFont::AlignBottom) { rect.Y = y + height - text_height; }
		else if (font.Align & UIFont::AlignVCenter) { rect.Y = y + std::round((height - text_height) * 0.5f); }
		// else if (font.Align & UIFont::AlignBaseline) { rect.Y = y + baseline; }
		else { rect.Y = y; }

		if (boundingRect) (*boundingRect) = UIRect{ rect.X, rect.Y, rect.W, rect.H };

		if (cursorRect)
		{
			*cursorRect = { rect.X + _cursorRect.X, rect.Y + _cursorRect.Y, _cursorRect.W, _cursorRect.H };
		}

		shape.setRotate(0);
		shape.setTranslate(rect.X, rect.Y);
		shape.setScale(1, 1);
		CONTEXT()->fillElement(&shape);
	}
}

UIPen const& SDLGPUPainter::getPen() const
{
	return PRIVATE()->Pen;
}

void SDLGPUPainter::setPen(const UIPen& pen)
{
	PRIVATE()->Pen = pen;
}

UIBrush const& SDLGPUPainter::getBrush() const
{
	return PRIVATE()->Brush;
}

void SDLGPUPainter::setBrush(const UIBrush& brush)
{
	PRIVATE()->Brush = brush;
}

UIFont const& SDLGPUPainter::getFont() const
{
	return PRIVATE()->Font;
}

void SDLGPUPainter::setFont(const UIFont& font)
{
	PRIVATE()->Font = font;
}

void SDLGPUPainter::setClipping(bool enable)
{
}

void SDLGPUPainter::setClipRect(float x, float y, float width, float height)
{
	PRIVATE()->Scissor = { x, y, width, height };
}

void SDLGPUPainter::setViewport(float x, float y, float width, float height)
{
	PRIVATE()->Viewport = { x, y, width, height };
}

void SDLGPUPainter::skew(float sh, float sv)
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

#endif