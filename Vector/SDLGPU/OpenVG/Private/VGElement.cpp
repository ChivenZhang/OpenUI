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
#include "../VGElement.h"

struct VGElementPrivate : VGPrivate
{
	float Rotation = 0;
	VGFloat2 Translate = { 0,0 }, Scaling = { 1,1 };
	VGFloat4 Scissor = { 0, 0, -1, -1 };
	VGPrimitiveRef ClipCache, FillCache, StrokeCache;
};
#define PRIVATE() ((VGElementPrivate*) m_Private)

VGElement::VGElement()
{
	m_Private = new VGElementPrivate;
}

VGElement::~VGElement()
{
	delete m_Private; m_Private = nullptr;
}

float VGElement::getRotate() const
{
	return PRIVATE()->Rotation;
}

void VGElement::setRotate(float value)
{
	PRIVATE()->Rotation = value;
}

VGFloat2 VGElement::getScale() const
{
	return PRIVATE()->Scaling;
}

void VGElement::setScale(float sx, float sy)
{
	PRIVATE()->Scaling = { sx, sy };
}

VGFloat2 VGElement::getTranslate() const
{
	return PRIVATE()->Translate;
}

void VGElement::setTranslate(float tx, float ty)
{
	PRIVATE()->Translate = { tx, ty };
}

VGFloat4 VGElement::getScissor() const
{
	return PRIVATE()->Scissor;
}

void VGElement::setScissor(float x, float y, float width, float height)
{
	PRIVATE()->Scissor = { x, y, width, height };
}

VGPrimitiveRaw VGElement::getClipCache() const
{
	return PRIVATE()->ClipCache.get();
}

void VGElement::setClipCache(VGPrimitiveRef value)
{
	PRIVATE()->ClipCache = value;
}

VGPrimitiveRaw VGElement::getFillCache() const
{
	return PRIVATE()->FillCache.get();
}

void VGElement::setFillCache(VGPrimitiveRef value)
{
	PRIVATE()->FillCache = value;
}

VGPrimitiveRaw VGElement::getStrokeCache() const
{
	return PRIVATE()->StrokeCache.get();
}

void VGElement::setStrokeCache(VGPrimitiveRef value)
{
	PRIVATE()->StrokeCache = value;
}
