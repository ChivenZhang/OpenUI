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
#include "UILabel.h"
#include "../UICanvas.h"
#include <stb_image_resize2.h>

struct UILabelPrivate : UIPrivate
{
	UILabelStyle Style;
	UIString Text;
	UIImage Image, ImageScaled;
	UIList<uint8_t> Pixel, PixelScaled;
	UILabel::scale_t ScaledContents = UILabel::scale_t::ScaleFixed;
	bool Hovered = false;
	UISignalAs<UIString> LinkHovered, LinkActivated;
};
#define PRIVATE() ((UILabelPrivate*)m_Private)

UILabel::UILabel(UICanvasRaw canvas)
	:
	UIWidget(canvas)
{
	m_Private = new UILabelPrivate;

	linkHovered = &PRIVATE()->LinkHovered;
	linkActivated = &PRIVATE()->LinkActivated;

	this->setEmbedAttrib("textContent", PRIVATE()->Text);
	setStyle(PRIVATE()->Style);
}

UILabel::~UILabel()
{
	delete m_Private; m_Private = nullptr;
}

void UILabel::layout(UIRect client)
{
	if (PRIVATE()->Image.Pixels == nullptr) return;
	auto rawPixels = PRIVATE()->Image.Pixels;
	auto rawWidth = PRIVATE()->Image.Width;
	auto rawHeight = PRIVATE()->Image.Height;
	auto rawStride = PRIVATE()->Image.Stride;

	if (PRIVATE()->Image.Pixels && PRIVATE()->Image.Format == UIImage::Byte)
	{
		switch (PRIVATE()->ScaledContents)
		{
		case ScaleFixed:
			{
				PRIVATE()->PixelScaled.clear();
				PRIVATE()->ImageScaled = UIImage{};
			} break;
		case ScaleAuto:
			{
				uint32_t newWidth = client.W;
				uint32_t newHeight = client.H;

				if (rawWidth * newHeight <= rawHeight * newWidth)
				{
					newWidth = client.W;
					newHeight = rawHeight * client.W / rawWidth;
				}
				else
				{
					newWidth = rawWidth * client.H / rawHeight;
					newHeight = client.H;
				}
				uint32_t newStride = newWidth * 4;
				PRIVATE()->PixelScaled.resize(newHeight * newStride);
				auto newPixels = PRIVATE()->PixelScaled.data();
				if (stbir_resize(rawPixels, rawWidth, rawHeight, rawStride,
					newPixels, newWidth, newHeight, newStride, STBIR_RGBA, STBIR_TYPE_UINT8, STBIR_EDGE_CLAMP, STBIR_FILTER_DEFAULT))
				{
					PRIVATE()->ImageScaled.Width = newWidth;
					PRIVATE()->ImageScaled.Height = newHeight;
					PRIVATE()->ImageScaled.Stride = newStride;
					PRIVATE()->ImageScaled.Channel = 4;
					PRIVATE()->ImageScaled.Pixels = PRIVATE()->PixelScaled.data();
				}
			} break;
		case ScaleNoRatio:
			{
				uint32_t newWidth = client.W;
				uint32_t newHeight = client.H;
				uint32_t newStride = newWidth * 4;
				PRIVATE()->PixelScaled.resize(newHeight * newStride);
				auto newPixels = PRIVATE()->PixelScaled.data();
				if (stbir_resize(rawPixels, rawWidth, rawHeight, rawStride,
					newPixels, newWidth, newHeight, newStride, STBIR_RGBA, STBIR_TYPE_UINT8, STBIR_EDGE_CLAMP, STBIR_FILTER_DEFAULT))
				{
					PRIVATE()->ImageScaled.Width = newWidth;
					PRIVATE()->ImageScaled.Height = newHeight;
					PRIVATE()->ImageScaled.Stride = newStride;
					PRIVATE()->ImageScaled.Channel = 4;
					PRIVATE()->ImageScaled.Pixels = PRIVATE()->PixelScaled.data();
				}
			} break;
		case ScaleKeepRatio:
			{
				uint32_t newWidth = client.W;
				uint32_t newHeight = client.H;
				if (rawWidth * newHeight <= rawHeight * newWidth)
				{
					newWidth = rawWidth * client.H / rawHeight;
					newHeight = client.H;
				}
				else
				{
					newWidth = client.W;
					newHeight = rawHeight * client.W / rawWidth;
				}
				uint32_t newStride = newWidth * 4;
				PRIVATE()->PixelScaled.resize(newHeight * newStride);
				auto newPixels = PRIVATE()->PixelScaled.data();
				if (stbir_resize(rawPixels, rawWidth, rawHeight, rawStride,
					newPixels, newWidth, newHeight, newStride, STBIR_BGRA, STBIR_TYPE_UINT8, STBIR_EDGE_CLAMP, STBIR_FILTER_DEFAULT))
				{
					PRIVATE()->ImageScaled.Width = newWidth;
					PRIVATE()->ImageScaled.Height = newHeight;
					PRIVATE()->ImageScaled.Stride = newStride;
					PRIVATE()->ImageScaled.Channel = 4;
					PRIVATE()->ImageScaled.Pixels = PRIVATE()->PixelScaled.data();
				}
			} break;
		}
	}
	else if (PRIVATE()->Image.Texture && PRIVATE()->Image.Format == UIImage::GPUByte)
	{
		PRIVATE()->ImageScaled = PRIVATE()->Image;
	}
}

void UILabel::paint(UIRect client, UIPainterRaw painter)
{
	UIWidget::paint(client, painter);

	auto style = getStyle();
	auto const& mode = getEnable() == false ? style.Disable : PRIVATE()->Hovered ? style.Hover : style.Normal;
	auto& widgetStyle = getEnable() == false ? getStyleDisabled() : PRIVATE()->Hovered ? getStyleHover() : getStyleNormal();
	auto face = UIFaceOf(widgetStyle, client);
	if (mode.Background.Pen.Style != UIPen::NoPen) face.Border = mode.Background.Pen;
	if (mode.Background.Brush.Style != UIBrush::NoBrush) face.Fill = mode.Background.Brush;
	UIDrawFace(painter, client, face);

	if (PRIVATE()->Image.Pixels)
	{
		switch (PRIVATE()->ScaledContents)
		{
		case ScaleFixed:
		{
			painter->drawImage(client.X, client.Y, PRIVATE()->Image);
		} break;
		case ScaleAuto:
		{
			auto newWidth = PRIVATE()->ImageScaled.Width;
			auto newHeight = PRIVATE()->ImageScaled.Height;
			painter->drawImage(client.X + 0.5f * (client.W - newWidth), client.Y + 0.5f * (client.H - newHeight), PRIVATE()->ImageScaled);
		} break;
		case ScaleNoRatio:
		{
			auto newWidth = PRIVATE()->ImageScaled.Width;
			auto newHeight = PRIVATE()->ImageScaled.Height;
			painter->drawImage(client.X + 0.5f * (client.W - newWidth), client.Y + 0.5f * (client.H - newHeight), PRIVATE()->ImageScaled);
		} break;
		case ScaleKeepRatio:
		{
			auto newWidth = PRIVATE()->ImageScaled.Width;
			auto newHeight = PRIVATE()->ImageScaled.Height;
			painter->drawImage(client.X + 0.5f * (client.W - newWidth), client.Y + 0.5f * (client.H - newHeight), PRIVATE()->ImageScaled);
		} break;
		}
	}

	auto textContent = this->getAttribText("textContent");
	if (textContent.empty() == false)
	{
		painter->setPen(UINoPen);
		painter->setFont(mode.Foreground.Font);
		painter->setBrush(mode.Foreground.Brush.Style == UIBrush::NoBrush ? face.Text : mode.Foreground.Brush);
		painter->drawText(client.X, client.Y, client.W, client.H, textContent);
	}
}

UIString& UILabel::getText() const
{
	return PRIVATE()->Text;
}

void UILabel::setText(UIString const& text)
{
	PRIVATE()->Text = text;
}

UILabelStyle UILabel::getStyle() const
{
	UILabelStyle result = PRIVATE()->Style;
	auto read = [&](UILabelStyle::LabelMode& mode, UIWidgetStyle const& src)
	{
		readFrame(src, mode.Background.Pen, mode.Background.Brush);
		applyFontFamily(src, mode.Foreground.Font);
		if (src.Foreground.Color.Type == UI_CSS_COLOR_CURRENTCOLOR) return;
		auto text = readColor(src.Foreground.Color);
		mode.Foreground.Brush = text.A <= 0.0f ? UIBrush{ .Style = UIBrush::NoBrush } : UIBrush{ .Color = text };
	};
	read(result.Normal, getStyleNormal());
	read(result.Hover, getStyleHover());
	read(result.Disable, getStyleDisabled());
	return result;
}

void UILabel::setStyle(UILabelStyle const& style)
{
	PRIVATE()->Style = style;
	auto write = [&](UIWidgetStyle& dst, UILabelStyle::LabelMode const& mode)
	{
		writeFrame(dst, mode.Background.Pen, mode.Background.Brush);
		assignFontFamily(dst, mode.Foreground.Font.Family);
		if (mode.Foreground.Brush.Style == UIBrush::NoBrush) dst.Foreground.Color = {};
		else writeColor(dst.Foreground.Color, mode.Foreground.Brush.Color);
	};
	write(getStyleNormal(), style.Normal);
	write(getStyleHover(), style.Hover);
	write(getStyleDisabled(), style.Disable);
}

UIFontAligns UILabel::getAlignment() const
{
	return PRIVATE()->Style.Normal.Foreground.Font.Align;
}

void UILabel::setAlignment(UIFontAligns value)
{
	PRIVATE()->Style.Normal.Foreground.Font.Align = value;
}

UIImage UILabel::getPixmap() const
{
	return PRIVATE()->Image;
}

void UILabel::setPixmap(UIImage image)
{
	if (image.Pixels && image.Format == UIImage::Byte && image.Width * 4 == image.Stride)
	{
		auto channel = image.Stride / image.Width;
		PRIVATE()->Pixel.resize(image.Width * image.Height * 4);
		PRIVATE()->Image = UIImage{ image.Width, image.Height, image.Width * 4, 0, PRIVATE()->Pixel.data() };
		auto dstPixels = PRIVATE()->Pixel.data();
		auto srcPixels = (uint8_t*)image.Pixels;
		auto numPixels = image.Width * image.Height;
		for (uint32_t i = 0; i < numPixels; ++i)
		{
			auto dstIndex = 4 * i;
			auto srcIndex = channel * i;
			dstPixels[dstIndex + 2] = (0 < channel) ? srcPixels[srcIndex] : 0;
			dstPixels[dstIndex + 1] = (1 < channel) ? srcPixels[srcIndex + 1] : 0;
			dstPixels[dstIndex + 0] = (2 < channel) ? srcPixels[srcIndex + 2] : 0;
			dstPixels[dstIndex + 3] = (3 < channel) ? srcPixels[srcIndex + 3] : 255;
		}
	}
	else if (image.Pixels && image.Format == UIImage::GPUByte && image.Width * 4 == image.Stride)
	{
		PRIVATE()->Pixel.clear();
		PRIVATE()->Image = image;
		PRIVATE()->PixelScaled.clear();
		PRIVATE()->ImageScaled = {};
	}
	else
	{
		PRIVATE()->Pixel.clear();
		PRIVATE()->Image = {};
		PRIVATE()->PixelScaled.clear();
		PRIVATE()->ImageScaled = {};
	}
}

UILabel::scale_t UILabel::getScaledContents() const
{
	return PRIVATE()->ScaledContents;
}

void UILabel::setScaledContents(scale_t value)
{
	PRIVATE()->ScaledContents = value;
}

void UILabel::mouseDoubleEvent(UIMouseEventRaw event)
{
	auto viewport = UIOverlap(getViewport(), getBounds());
	if (viewport.X <= event->X && event->X <= viewport.X + viewport.W
		&& viewport.Y <= event->Y && event->Y <= viewport.Y + viewport.H)
	{
		if (event->Button == UIInputEnum::MOUSE_BUTTON_LEFT)
		{
			PRIVATE()->LinkActivated.signal(PRIVATE()->Text);
		}
	}
}

void UILabel::mousePressEvent(UIMouseEventRaw event)
{
	auto viewport = UIOverlap(getViewport(), getBounds());
	if (viewport.X <= event->X && event->X <= viewport.X + viewport.W
		&& viewport.Y <= event->Y && event->Y <= viewport.Y + viewport.H)
	{
		if (event->Button == UIInputEnum::MOUSE_BUTTON_LEFT)
		{
			PRIVATE()->LinkActivated.signal(PRIVATE()->Text);
		}
	}
}

void UILabel::mouseReleaseEvent(UIMouseEventRaw event)
{
}

void UILabel::mouseMoveEvent(UIMouseEventRaw event)
{
	if (inBounds(event->X, event->Y))
	{
		PRIVATE()->Hovered = true;

		PRIVATE()->LinkHovered.signal(PRIVATE()->Text);
	}
	else
	{
		PRIVATE()->Hovered = false;
	}
}

void UILabel::enterEvent(UIMouseEventRaw event)
{
}

void UILabel::leaveEvent(UIMouseEventRaw event)
{
	PRIVATE()->Hovered = false;
}