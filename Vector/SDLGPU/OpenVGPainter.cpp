/*=================================================
* Copyright © 2020-2025 ChivenZhang.
* All Rights Reserved.
* =====================Note=========================
*
*
* ====================History=======================
* Created by ChivenZhang@gmail.com.
*
* =================================================*/
#include "OpenVGPainter.h"
#include "SDLGPURenderVG.h"
#include <OpenVG/VGContext.h>
#include <OpenVG/VGShape.h>
#include <OpenVG/VGText.h>
#include <OpenVG/VGPicture.h>
#include <OpenVG/VGTrueType.h>

class OpenVGPainterData : public UIPainterPrivate
{
public:
	VGRect Client;
	VGContextRef Context;
	UIPen Pen;
	UIFont Font;
	UIBrush Brush;
	UIRect Scissor, Viewport;
	bool EnableCilp = false;
	VGShapeRef RectFillShape;
};
#define PRIVATE() ((OpenVGPainterData*) m_Private)
#define CONTEXT() (PRIVATE()->Context)

OpenVGPainter::OpenVGPainter(uint32_t width, uint32_t height)
{
	m_Private = new OpenVGPainterData;
	PRIVATE()->Context = VGNew<VGContext>();
	CONTEXT()->setPainter(VGNew<VGPainter>());
	CONTEXT()->setRender(VGNew<SDLGPURenderVG>());
}

OpenVGPainter::~OpenVGPainter()
{
	delete m_Private; m_Private = nullptr;
}

UIRect OpenVGPainter::boundingRect(float x, float y, float width, float height, UIString const& text, float cursor, UIRectRaw cursorRect)
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

UIRect OpenVGPainter::boundingRect(float x, float y, float width, float height, UIString const& text, float posX, float posY, int* cursor, UIRectRaw cursorRect)
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

void OpenVGPainter::drawArc(float x, float y, float width, float height, float startAngle, float spanAngle)
{
	if (PRIVATE()->Brush.Style != UIBrush::NoBrush)
	{
		VGShape shape;
		shape.moveTo(x, y);
		shape.arcTo(width * 0.5f, height * 0.5f, width * 0.5f, height * 0.5f, startAngle, spanAngle);
		shape.close();
		auto color = getBrush().Color;
		shape.setFillColor({ color.R, color.G, color.B, color.A });
		auto scissor = PRIVATE()->Scissor;
		shape.setScissor({ scissor.X, scissor.Y, scissor.W, scissor.H });
		CONTEXT()->fillElement(&shape);
	}
	if (PRIVATE()->Pen.Style != UIPen::NoPen)
	{
		VGShape shape;
		shape.moveTo(x, y);
		shape.arcTo(width * 0.5f, height * 0.5f, width * 0.5f, height * 0.5f, startAngle, spanAngle);
		shape.close();
		auto color = getPen().Color;
		shape.setStrokeColor({ color.R, color.G, color.B, color.A });
		auto scissor = PRIVATE()->Scissor;
		shape.setScissor({ scissor.X, scissor.Y, scissor.W, scissor.H });
		CONTEXT()->strokeElement(&shape);
	}
}

void OpenVGPainter::drawChord(float x, float y, float width, float height, float startAngle, float spanAngle)
{
}

void OpenVGPainter::drawEllipse(float x, float y, float width, float height)
{
	if (width <= 0 || height <= 0) return;
	if (PRIVATE()->Brush.Style != UIBrush::NoBrush)
	{
		VGShape shape;
		shape.arcTo(x + width * 0.5f, y + height * 0.5f, width * 0.5f, height * 0.5f, 0, 360);
		shape.close();
		auto color = getBrush().Color;
		shape.setFillColor({ color.R, color.G, color.B, color.A });
		auto scissor = PRIVATE()->Scissor;
		shape.setScissor({ scissor.X, scissor.Y, scissor.W, scissor.H });
		CONTEXT()->fillElement(&shape);
	}
	if (PRIVATE()->Pen.Style != UIPen::NoPen)
	{
		VGShape shape;
		shape.arcTo(x + width * 0.5f, y + height * 0.5f, width * 0.5f, height * 0.5f, 0, 360);
		shape.close();
		shape.setLineWidth(getPen().Width);
		auto color = getPen().Color;
		shape.setStrokeColor({ color.R, color.G, color.B, color.A });
		auto scissor = PRIVATE()->Scissor;
		shape.setScissor({ scissor.X, scissor.Y, scissor.W, scissor.H });
		CONTEXT()->strokeElement(&shape);
	}
}

void OpenVGPainter::drawImage(float x, float y, UIImage image, float sx, float sy, float sw, float sh)
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
		_image.Pixel = image.Pixel;
		_image.Type = (VGImage::type_t)image.Type;
		shape.setImage(0, 0, image.Width, image.Height, _image);

		shape.setRotate(0);
		shape.setTranslate(x, y);
		shape.setScale(1, 1);
		CONTEXT()->fillElement(&shape);
	}
}

void OpenVGPainter::drawLine(float x1, float y1, float x2, float y2)
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

void OpenVGPainter::drawLines(UIArrayView<UILine> lines)
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

void OpenVGPainter::drawPie(float x, float y, float width, float height, float startAngle, float spanAngle)
{
	if (width <= 0 || height <= 0) return;
	if (PRIVATE()->Brush.Style != UIBrush::NoBrush)
	{
		VGShape shape;
		shape.moveTo(0, 0);
		shape.arcTo(0, 0, width * 0.5f, height * 0.5f, startAngle, spanAngle);
		shape.close();
		auto color = getBrush().Color;
		shape.setFillColor({ color.R, color.G, color.B, color.A });
		auto scissor = PRIVATE()->Scissor;
		shape.setScissor({ scissor.X, scissor.Y, scissor.W, scissor.H });
		CONTEXT()->fillElement(&shape);
	}
	if (PRIVATE()->Pen.Style != UIPen::NoPen)
	{
		VGShape shape;
		shape.moveTo(0, 0);
		shape.arcTo(0, 0, width * 0.5f, height * 0.5f, startAngle, spanAngle);
		shape.close();
		shape.setLineWidth(getPen().Width);
		auto color = getPen().Color;
		shape.setStrokeColor({ color.R, color.G, color.B, color.A });
		auto scissor = PRIVATE()->Scissor;
		shape.setScissor({ scissor.X, scissor.Y, scissor.W, scissor.H });
		CONTEXT()->strokeElement(&shape);
	}
}

void OpenVGPainter::drawPoint(float x, float y)
{
	drawRect(x, y, 1, 1);
}

void OpenVGPainter::drawPoints(UIArrayView<UIPoint> points)
{
	for (size_t i = 0; i < points.size(); ++i)
	{
		drawRect(points[i].X, points[i].Y, 1, 1);
	}
}

void OpenVGPainter::drawPolygon(UIArrayView<UIPoint> points)
{
	if (points.size() < 3) return;
	if (PRIVATE()->Brush.Style != UIBrush::NoBrush)
	{
		VGShape shape;
		shape.moveTo(points.front().X, points.front().Y);
		for (size_t i = 1; i < points.size(); ++i)
			shape.lineTo(points[i].X, points[i].Y);
		shape.lineTo(points.front().X, points.front().Y);
		shape.close();
		auto color = getBrush().Color;
		shape.setFillColor({ color.R, color.G, color.B, color.A });
		auto scissor = PRIVATE()->Scissor;
		shape.setScissor({ scissor.X, scissor.Y, scissor.W, scissor.H });
		CONTEXT()->fillElement(&shape);
	}
	if (PRIVATE()->Pen.Style != UIPen::NoPen)
	{
		VGShape shape;
		shape.moveTo(points.front().X, points.front().Y);
		for (size_t i = 1; i < points.size(); ++i)
			shape.lineTo(points[i].X, points[i].Y);
		shape.lineTo(points.front().X, points.front().Y);
		shape.close();
		shape.setLineWidth(getPen().Width);
		auto color = getPen().Color;
		shape.setStrokeColor({ color.R, color.G, color.B, color.A });
		auto scissor = PRIVATE()->Scissor;
		shape.setScissor({ scissor.X, scissor.Y, scissor.W, scissor.H });
		CONTEXT()->strokeElement(&shape);
	}
}

void OpenVGPainter::drawPolyline(UIArrayView<UIPoint> points)
{
	if (points.size() < 2) return;
	if (PRIVATE()->Pen.Style != UIPen::NoPen)
	{
		VGShape shape;
		shape.moveTo(points.front().X, points.front().Y);
		for (size_t i = 1; i < points.size(); ++i)
			shape.lineTo(points[i].X, points[i].Y);
		shape.close();
		shape.setLineWidth(getPen().Width);
		auto color = getPen().Color;
		shape.setStrokeColor({ color.R, color.G, color.B, color.A });
		auto scissor = PRIVATE()->Scissor;
		shape.setScissor({ scissor.X, scissor.Y, scissor.W, scissor.H });
		CONTEXT()->strokeElement(&shape);
	}
}

void OpenVGPainter::drawRect(float x, float y, float width, float height)
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

void OpenVGPainter::drawRects(UIArrayView<UIRect> rects)
{
	for (size_t i = 0; i < rects.size(); ++i)
	{
		drawRect(rects[i].X, rects[i].Y, rects[i].W, rects[i].H);
	}
}

void OpenVGPainter::drawRoundedRect(float x, float y, float width, float height, float xRadius, float yRadius)
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

void OpenVGPainter::drawText(float x, float y, float width, float height, const UIString& text, UIRectRaw boundingRect, float cursor, UIRectRaw cursorRect)
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

UIPen const& OpenVGPainter::getPen() const
{
	return PRIVATE()->Pen;
}

void OpenVGPainter::setPen(const UIPen& pen)
{
	PRIVATE()->Pen = pen;
}

UIBrush const& OpenVGPainter::getBrush() const
{
	return PRIVATE()->Brush;
}

void OpenVGPainter::setBrush(const UIBrush& brush)
{
	PRIVATE()->Brush = brush;
}

UIFont const& OpenVGPainter::getFont() const
{
	return PRIVATE()->Font;
}

void OpenVGPainter::setFont(const UIFont& font)
{
	PRIVATE()->Font = font;
}

void OpenVGPainter::setClipping(bool enable)
{
}

void OpenVGPainter::setClipRect(float x, float y, float width, float height)
{
	PRIVATE()->Scissor = { x, y, width, height };
}

void OpenVGPainter::setViewport(float x, float y, float width, float height)
{
	PRIVATE()->Viewport = { x, y, width, height };
}

void OpenVGPainter::shear(float sh, float sv)
{
}

void OpenVGPainter::rotate(float angle)
{
}

void OpenVGPainter::scale(float dx, float dy)
{
}

void OpenVGPainter::translate(float dx, float dy)
{
}

uint32_t OpenVGPainter::getWidth() const
{
	return PRIVATE()->Client.W;
}

uint32_t OpenVGPainter::getHeight() const
{
	return PRIVATE()->Client.H;
}

uint32_t OpenVGPainter::getStride() const
{
	return getWidth() * 4;
}

UIArrayView<const uint8_t> OpenVGPainter::getPixels() const
{
	return UIArrayView<const uint8_t>();
}

void OpenVGPainter::resize(uint32_t width, uint32_t height)
{
	PRIVATE()->Client = VGRect{ 0,0,(float)width,(float)height };
}

VGContextRaw OpenVGPainter::getVGContext() const
{
	return PRIVATE()->Context.get();
}
