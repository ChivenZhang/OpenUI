#include "RmGUILabel.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb_image_resize2.h>

class RmGUILabelPrivate : public RmGUIWidgetPrivate
{
public:
	RmGUILabelStyle Style;
	RmString Text;
	RmImage Image, ImageScaled;
	RmVector<uint8_t> ImageData, ImageDataScaled;
	RmGUILabel::scale_t ScaledContents = RmGUILabel::ScaleFixed;
	RmGUISignalAs<RmString> LinkHovered, LinkActivated;
};
#define PRIVATE() ((RmGUILabelPrivate*)m_PrivateLabel)

RmGUILabel::RmGUILabel(IRmGUIWidgetRaw parent)
	:
	RmGUIControl(parent),
	m_PrivateLabel(nullptr),
	linkHovered(nullptr),
	linkActivated(nullptr)
{
	m_PrivateLabel = new RmGUILabelPrivate;
	PRIVATE()->Style.Pen = { .Style = RmPen::NoPen, };
	PRIVATE()->Style.Font = { .Align = RmFont::AlignLeft | RmFont::AlignVCenter, };
	PRIVATE()->Style.Brush = {};
	linkHovered = &PRIVATE()->LinkHovered;
	linkActivated = &PRIVATE()->LinkActivated;
}

RmGUILabel::~RmGUILabel()
{
	delete m_PrivateLabel; m_PrivateLabel = nullptr;
}

void RmGUILabel::layout(RmRectRaw client)
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
		PRIVATE()->ImageScaled = RmImage{};
	} break;
	case ScaleAuto:
	{
		uint32_t newWidth = client->W;
		uint32_t newHeight = client->H;

		if (rawWidth * newHeight <= rawHeight * newWidth)
		{
			newWidth = client->W;
			newHeight = rawHeight * client->W / rawWidth;
		}
		else
		{
			newWidth = rawWidth * client->H / rawHeight;
			newHeight = client->H;
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
		uint32_t newWidth = client->W;
		uint32_t newHeight = client->H;
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
		uint32_t newWidth = client->W;
		uint32_t newHeight = client->H;
		if (rawWidth * newHeight <= rawHeight * newWidth)
		{
			newWidth = rawWidth * client->H / rawHeight;
			newHeight = client->H;
		}
		else
		{
			newWidth = client->W;
			newHeight = rawHeight * client->W / rawWidth;
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

void RmGUILabel::paint(IRmGUIPainterRaw painter, RmRectRaw client)
{
	RmGUIWidget::paint(painter, client);
	if (PRIVATE()->Image.Data.size())
	{
		switch (PRIVATE()->ScaledContents)
		{
		case ScaleFixed:
		{
			painter->drawImage(client->X, client->Y, PRIVATE()->Image);
		} break;
		case ScaleAuto:
		{
			auto newWidth = PRIVATE()->ImageScaled.Width;
			auto newHeight = PRIVATE()->ImageScaled.Height;
			painter->drawImage(client->X + 0.5f * (client->W - newWidth), client->Y + 0.5f * (client->H - newHeight), PRIVATE()->ImageScaled);
		} break;
		case ScaleNoRatio:
		{
			auto newWidth = PRIVATE()->ImageScaled.Width;
			auto newHeight = PRIVATE()->ImageScaled.Height;
			painter->drawImage(client->X + 0.5f * (client->W - newWidth), client->Y + 0.5f * (client->H - newHeight), PRIVATE()->ImageScaled);
		} break;
		case ScaleKeepRatio:
		{
			auto newWidth = PRIVATE()->ImageScaled.Width;
			auto newHeight = PRIVATE()->ImageScaled.Height;
			painter->drawImage(client->X + 0.5f * (client->W - newWidth), client->Y + 0.5f * (client->H - newHeight), PRIVATE()->ImageScaled);
		} break;
		}
	}
	if (PRIVATE()->Text.empty() == false)
	{
		painter->setPen(PRIVATE()->Style.Pen);
		painter->setFont(PRIVATE()->Style.Font);
		painter->setBrush(PRIVATE()->Style.Brush);
		painter->drawText(client->X, client->Y, client->W, client->H, PRIVATE()->Text);
	}
}

RmString RmGUILabel::getText() const
{
	return PRIVATE()->Text;
}

void RmGUILabel::setText(RmString const& text)
{
	PRIVATE()->Text = text;
}

RmGUILabelStyle RmGUILabel::getStyle() const
{
	return PRIVATE()->Style;
}

void RmGUILabel::setStyle(RmGUILabelStyle const& style)
{
	PRIVATE()->Style = style;
}

RmFontAligns RmGUILabel::getAlignment() const
{
	return PRIVATE()->Style.Font.Align;
}

void RmGUILabel::setAlignment(RmFontAligns value)
{
	PRIVATE()->Style.Font.Align = value;
}

RmImage RmGUILabel::getPixmap() const
{
	return PRIVATE()->Image;
}

void RmGUILabel::setPixmap(RmImage image)
{
	if (image.Height * image.Stride == image.Data.size())
	{
		PRIVATE()->ImageData.resize(image.Data.size());
		::memcpy(PRIVATE()->ImageData.data(), image.Data.data(), image.Data.size());
		PRIVATE()->Image = RmImage{ image.Width, image.Height, image.Stride, PRIVATE()->ImageData };
		auto pixels = PRIVATE()->ImageData.data();
		int num_pixels = image.Width * image.Height;
		for (int i = 0; i < num_pixels; ++i)
		{
			auto pixel = pixels + i * 4;
			std::swap(pixel[0], pixel[2]);
		}
	}
	else
	{
		PRIVATE()->ImageData.clear();
		PRIVATE()->Image = RmImage{};
	}
}

RmGUILabel::scale_t RmGUILabel::getScaledContents() const
{
	return PRIVATE()->ScaledContents;
}

void RmGUILabel::setScaledContents(scale_t value)
{
	PRIVATE()->ScaledContents = value;
}

void RmGUILabel::mousePressEvent(IRmGUIMouseEventRaw event)
{
	auto client = getRect();
	auto viewport = getViewport();
	if (client.X <= event->X && event->X <= client.X + client.W
		&& client.Y <= event->Y && event->Y <= client.Y + client.H
		&& viewport.X <= event->X && event->X <= viewport.X + viewport.W
		&& viewport.Y <= event->Y && event->Y <= viewport.Y + viewport.H)
	{
		if (event->Button == 1)
		{
			PRIVATE()->LinkActivated.emit(PRIVATE()->Text);
		}
	}
}

void RmGUILabel::mouseReleaseEvent(IRmGUIMouseEventRaw event)
{
}

void RmGUILabel::mouseDoubleEvent(IRmGUIMouseEventRaw event)
{
}

void RmGUILabel::mouseMoveEvent(IRmGUIMouseEventRaw event)
{
	auto client = getRect();
	auto viewport = getViewport();
	if (client.X <= event->X && event->X <= client.X + client.W
		&& client.Y <= event->Y && event->Y <= client.Y + client.H
		&& viewport.X <= event->X && event->X <= viewport.X + viewport.W
		&& viewport.Y <= event->Y && event->Y <= viewport.Y + viewport.H)
	{
		PRIVATE()->LinkHovered.emit(PRIVATE()->Text);
	}
}