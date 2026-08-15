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
#include "../VGPicture.h"

struct VGPicturePrivate : VGPrivate
{
	VGImage Image;
	VGRect Client;
	VGColor Color = { 1,1,1,1 };
};
#define PRIVATE() ((VGPicturePrivate*) m_Private)

VGPicture::VGPicture()
{
	m_Private = new VGPicturePrivate;
}

VGPicture::~VGPicture()
{
	delete m_Private; m_Private = nullptr;
}

VGColor VGPicture::getFillColor() const
{
	return PRIVATE()->Color;
}

void VGPicture::setFillColor(VGColor value)
{
	PRIVATE()->Color = value;
}

VGImage VGPicture::getImage() const
{
	return PRIVATE()->Image;
}

void VGPicture::setImage(float x, float y, float width, float height, VGImage image)
{
	setClipCache(nullptr);
	setFillCache(nullptr);
	setStrokeCache(nullptr);
	PRIVATE()->Image = image;
	PRIVATE()->Client = { x, y, width, height };
}

void VGPicture::clip()
{
}

void VGPicture::fill()
{
	if (getFillCache() == nullptr)
	{
		auto cache = VGNew<VGPrimitive>();

		auto& points = cache->PointList;
		auto& styles = cache->StyleList;
		auto& images = cache->ImageList;
		auto& linears = cache->LinearList;
		auto& radials = cache->RadialList;
		auto& matrixs = cache->MatrixList;
		auto& scissors = cache->ScissorList;

		auto& image = PRIVATE()->Image;
		auto& client = PRIVATE()->Client;
		auto& style = styles.emplace_back();
		auto& matrix = matrixs.emplace_back();
		matrix.Scissor = getScissor();
		matrix.Transform = VGFloat3x3::Transform(getTranslate().X, getTranslate().Y, getRotate(), getScale().X, getScale().Y);

		style.Color = PRIVATE()->Color;
		if (image.Pixels)
		{
			style.Image = (int32_t)images.size();
			style.Flags |= (VG_FLAGS_IMAGE_COLOR);
			if (image.Format == VGImage::HWByte || image.Format == VGImage::HWFloat) style.Flags |= (VG_FLAGS_IMAGE_HWGPU);

			images.emplace_back(image);
		}

		if ((style.Flags & VG_FLAGS_IMAGE_HWGPU) == VG_FLAGS_IMAGE_HWGPU)
		{
			points.push_back({ client.X, client.Y, 0, 1 - 0, 0, 0 });
			points.push_back({ client.X, client.Y + client.H, 0, 1 - 1, 0, 0 });
			points.push_back({ client.X + client.W, client.Y + client.H, 1, 1 - 1, 0, 0 });
			points.push_back({ client.X, client.Y, 0, 1 - 0, 0, 0 });
			points.push_back({ client.X + client.W, client.Y + client.H, 1, 1 - 1, 0, 0 });
			points.push_back({ client.X + client.W, client.Y, 1, 1 - 0, 0, 0 });
		}
		else
		{
			points.push_back({ client.X, client.Y, 0, 0, 0, 0 });
			points.push_back({ client.X, client.Y + client.H, 0, 1, 0, 0 });
			points.push_back({ client.X + client.W, client.Y + client.H, 1, 1, 0, 0 });
			points.push_back({ client.X, client.Y, 0, 0, 0, 0 });
			points.push_back({ client.X + client.W, client.Y + client.H, 1, 1, 0, 0 });
			points.push_back({ client.X + client.W, client.Y, 1, 0, 0, 0 });
		}

		setFillCache(cache);
	}
}

void VGPicture::stroke()
{
}
