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

void convertToRGBA(unsigned char* img, int width, int height, int* channels) {
	if (*channels == 4) {
		// 如果已经是RGBA，则无需转换  
		return;
	}

	// 分配新的内存来存储RGBA数据  
	unsigned char* rgbaImg = (unsigned char*)malloc(width * height * 4);
	if (!rgbaImg) {
		// 内存分配失败处理  
		return;
	}

	// 复制像素数据，并添加alpha通道  
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int index = y * width * *channels + x * *channels;
			int rgbaIndex = y * width * 4 + x * 4;

			// 复制RGB  
			for (int c = 0; c < *channels; c++) {
				rgbaImg[rgbaIndex + c] = img[index + c];
			}

			// 设置alpha为255（完全不透明）  
			rgbaImg[rgbaIndex + 3] = 255;
		}
	}

	// 释放原始图片内存（如果你不再需要它）  
	free(img);

	// 更新指针和通道数  
	img = rgbaImg;
	*channels = 4;
}

void RmGUILabel::setPixmap(RmImage image)
{
	if (image.Height * image.Stride == image.Data.size())
	{
		auto channel = image.Stride / image.Width;
		PRIVATE()->ImageData.resize(image.Width * image.Height * 4);
		PRIVATE()->Image = RmImage{ image.Width, image.Height, image.Width * 4, PRIVATE()->ImageData };
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
	auto viewport = RmOverlap(getViewport(), getRect());
	if (viewport.X <= event->X && event->X <= viewport.X + viewport.W
		&& viewport.Y <= event->Y && event->Y <= viewport.Y + viewport.H)
	{
		if (event->Button == 1)
		{
			PRIVATE()->LinkActivated.signal(PRIVATE()->Text);
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
	auto viewport = RmOverlap(getViewport(), getRect());
	if (viewport.X <= event->X && event->X <= viewport.X + viewport.W
		&& viewport.Y <= event->Y && event->Y <= viewport.Y + viewport.H)
	{
		PRIVATE()->LinkHovered.signal(PRIVATE()->Text);
	}
}