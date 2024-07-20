#include "UILabel.h"
#include "UILabel.h"
#include "UILabel.h"
#include "UILabel.h"
#include "UILabel.h"
#include "UILabel.h"
#include "UILabel.h"
#include "UILabel.h"
#include "UILabel.h"
#include "UILabel.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb_image_resize2.h>

/// @brief 
class UILabelPrivate : public UIElementPrivate
{
public:
	UILabelStyle Style;
	UIString Text;
	UIImage Image, ImageScaled;
	UIVector<uint8_t> ImageData, ImageDataScaled;
	UILabel::scale_t ScaledContents = UILabel::scale_t::ScaleFixed;
	bool Hovered = false;
	UISignalAs<UIString> LinkHovered, LinkActivated;
};
#define PRIVATE() ((UILabelPrivate*)m_PrivateLabel)

UILabel::UILabel()
{
	m_PrivateLabel = new UILabelPrivate;

	linkHovered = &PRIVATE()->LinkHovered;
	linkActivated = &PRIVATE()->LinkActivated;

	auto style = getStyle();
	style.Normal.Foreground.Pen = style.Hover.Foreground.Pen = style.Disable.Foreground.Pen = { .Style = UIPen::NoPen, };
	style.Normal.Foreground.Font = style.Hover.Foreground.Font = style.Disable.Foreground.Font = { .Align = UIFont::AlignLeft | UIFont::AlignVCenter, };
	style.Normal.Foreground.Brush = style.Hover.Foreground.Brush = style.Disable.Foreground.Brush = { .Color = {0,0,0,1}, };
	style.Normal.Background.Pen = style.Hover.Background.Pen = style.Disable.Background.Pen = { .Color = { 108 / 255.0f, 110 / 255.0f, 111 / 255.0f, 1.0f }, };
	style.Normal.Background.Brush = style.Hover.Background.Brush = style.Disable.Background.Brush = { .Color = { 238 / 255.0f, 238 / 255.0f, 242 / 255.0f, 1.0f }, };
	setStyle(style);
}

UILabel::~UILabel()
{
	delete m_PrivateLabel; m_PrivateLabel = nullptr;
}

void UILabel::layout(UIRect client)
{
	if (PRIVATE()->Image.Data.empty()) return;
	auto rawPixels = PRIVATE()->Image.Data.data();
	auto rawWidth = PRIVATE()->Image.Width;
	auto rawHeight = PRIVATE()->Image.Height;
	auto rawStride = PRIVATE()->Image.Stride;

	switch (PRIVATE()->ScaledContents)
	{
	case ScaleFixed:
	{
		PRIVATE()->ImageDataScaled.clear();
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
		PRIVATE()->ImageDataScaled.resize(newHeight * newStride);
		auto newPixels = PRIVATE()->ImageDataScaled.data();
		if (stbir_resize(rawPixels, rawWidth, rawHeight, rawStride,
			newPixels, newWidth, newHeight, newStride, STBIR_RGBA, STBIR_TYPE_UINT8, STBIR_EDGE_CLAMP, STBIR_FILTER_DEFAULT))
		{
			PRIVATE()->ImageScaled.Width = newWidth;
			PRIVATE()->ImageScaled.Height = newHeight;
			PRIVATE()->ImageScaled.Stride = newStride;
			PRIVATE()->ImageScaled.Data = PRIVATE()->ImageDataScaled;
		}
	} break;
	case ScaleNoRatio:
	{
		uint32_t newWidth = client.W;
		uint32_t newHeight = client.H;
		uint32_t newStride = newWidth * 4;
		PRIVATE()->ImageDataScaled.resize(newHeight * newStride);
		auto newPixels = PRIVATE()->ImageDataScaled.data();
		if (stbir_resize(rawPixels, rawWidth, rawHeight, rawStride,
			newPixels, newWidth, newHeight, newStride, STBIR_RGBA, STBIR_TYPE_UINT8, STBIR_EDGE_CLAMP, STBIR_FILTER_DEFAULT))
		{
			PRIVATE()->ImageScaled.Width = newWidth;
			PRIVATE()->ImageScaled.Height = newHeight;
			PRIVATE()->ImageScaled.Stride = newStride;
			PRIVATE()->ImageScaled.Data = PRIVATE()->ImageDataScaled;
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
		PRIVATE()->ImageDataScaled.resize(newHeight * newStride);
		auto newPixels = PRIVATE()->ImageDataScaled.data();
		if (stbir_resize(rawPixels, rawWidth, rawHeight, rawStride,
			newPixels, newWidth, newHeight, newStride, STBIR_BGRA, STBIR_TYPE_UINT8, STBIR_EDGE_CLAMP, STBIR_FILTER_DEFAULT))
		{
			PRIVATE()->ImageScaled.Width = newWidth;
			PRIVATE()->ImageScaled.Height = newHeight;
			PRIVATE()->ImageScaled.Stride = newStride;
			PRIVATE()->ImageScaled.Data = PRIVATE()->ImageDataScaled;
		}
	} break;
	}
}

void UILabel::paint(UIRect client, UIPainterRaw painter)
{
	UIElement::paint(client, painter);

	if (getEnable() == false)
	{
		painter->setPen(PRIVATE()->Style.Disable.Background.Pen);
		painter->setBrush(PRIVATE()->Style.Disable.Background.Brush);
	}
	else if (PRIVATE()->Hovered)
	{
		painter->setPen(PRIVATE()->Style.Hover.Background.Pen);
		painter->setBrush(PRIVATE()->Style.Hover.Background.Brush);
	}
	else
	{
		painter->setPen(PRIVATE()->Style.Normal.Background.Pen);
		painter->setBrush(PRIVATE()->Style.Normal.Background.Brush);
	}
	painter->drawRect(client.X + 1, client.Y + 1, client.W - 2, client.H - 2);

	if (PRIVATE()->Image.Data.size())
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
	if (PRIVATE()->Text.empty() == false)
	{
		if (getEnable() == false)
		{
			painter->setPen(PRIVATE()->Style.Disable.Foreground.Pen);
			painter->setFont(PRIVATE()->Style.Disable.Foreground.Font);
			painter->setBrush(PRIVATE()->Style.Disable.Foreground.Brush);
		}
		else if (PRIVATE()->Hovered)
		{
			painter->setPen(PRIVATE()->Style.Hover.Foreground.Pen);
			painter->setFont(PRIVATE()->Style.Hover.Foreground.Font);
			painter->setBrush(PRIVATE()->Style.Hover.Foreground.Brush);
		}
		else
		{
			painter->setPen(PRIVATE()->Style.Normal.Foreground.Pen);
			painter->setFont(PRIVATE()->Style.Normal.Foreground.Font);
			painter->setBrush(PRIVATE()->Style.Normal.Foreground.Brush);
		}
		painter->drawText(client.X, client.Y, client.W, client.H, PRIVATE()->Text);
	}
}

UIString UILabel::getText() const
{
	return PRIVATE()->Text;
}

void UILabel::setText(UIString const& text)
{
	PRIVATE()->Text = text;
}

UILabelStyle UILabel::getStyle() const
{
	return PRIVATE()->Style;
}

void UILabel::setStyle(UILabelStyle const& style)
{
	PRIVATE()->Style = style;
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
	if (image.Height * image.Stride == image.Data.size())
	{
		auto channel = image.Stride / image.Width;
		PRIVATE()->ImageData.resize(image.Width * image.Height * 4);
		PRIVATE()->Image = UIImage{ image.Width, image.Height, image.Width * 4, PRIVATE()->ImageData };
		auto pixels = PRIVATE()->ImageData.data();
		auto numPixels = image.Width * image.Height;
		for (int i = 0; i < numPixels; ++i)
		{
			auto toIndex = 4 * i;
			auto fromIndex = channel * i;
			pixels[toIndex + 2] = (0 < channel) ? image.Data[fromIndex] : 0;
			pixels[toIndex + 1] = (1 < channel) ? image.Data[fromIndex + 1] : 0;
			pixels[toIndex + 0] = (2 < channel) ? image.Data[fromIndex + 2] : 0;
			pixels[toIndex + 3] = (3 < channel) ? image.Data[fromIndex + 3] : 255;
		}
	}
	else
	{
		PRIVATE()->ImageData.clear();
		PRIVATE()->Image = UIImage{};
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
		if (event->Button == 1)
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
		if (event->Button == 1)
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

UIString UILabelFactory::getTagName() const
{
	return "label";
}

UIElementRef UILabelFactory::getElement(UIString style) const
{
	auto result = UINew<UILabel>();
	result->setStyleText(style);
	return result;
}
