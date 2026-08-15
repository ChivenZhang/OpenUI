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
#include "../VGPainter.h"
#include "../VGTessellate.h"

struct VGPainterPrivate : VGPrivate
{
	VGList<VGPrimitive> PrimitiveList;
};
#define PRIVATE() ((VGPainterPrivate*) m_Private)

VGPainter::VGPainter()
{
	m_Private = new VGPainterPrivate;
}

VGPainter::~VGPainter()
{
	delete m_Private; m_Private = nullptr;
}

void VGPainter::clip(VGElementRaw element)
{
	element->clip();
	if (element->getClipCache() == nullptr || element->getClipCache()->PointList.empty()) return;
	auto& primitive = PRIVATE()->PrimitiveList.emplace_back(*element->getClipCache());
	auto& matrix = primitive.MatrixList.back();
	matrix.Scissor = element->getScissor();
	matrix.Transform = VGFloat3x3::Transform(element->getTranslate().X, element->getTranslate().Y, element->getRotate(), element->getScale().X, element->getScale().Y);
}

void VGPainter::fill(VGElementRaw element)
{
	element->fill();
	if (element->getFillCache() == nullptr || element->getFillCache()->PointList.empty()) return;
	auto& primitive = PRIVATE()->PrimitiveList.emplace_back(*element->getFillCache());
	auto& matrix = primitive.MatrixList.back();
	matrix.Scissor = element->getScissor();
	matrix.Transform = VGFloat3x3::Transform(element->getTranslate().X, element->getTranslate().Y, element->getRotate(), element->getScale().X, element->getScale().Y);
}

void VGPainter::stroke(VGElementRaw element)
{
	element->stroke();
	if (element->getStrokeCache() == nullptr || element->getStrokeCache()->PointList.empty()) return;
	auto& primitive = PRIVATE()->PrimitiveList.emplace_back(*element->getStrokeCache());
	auto& matrix = primitive.MatrixList.back();
	matrix.Scissor = element->getScissor();
	matrix.Transform = VGFloat3x3::Transform(element->getTranslate().X, element->getTranslate().Y, element->getRotate(), element->getScale().X, element->getScale().Y);
}

VGList<VGPrimitive>& VGPainter::getPrimitiveList()
{
	return PRIVATE()->PrimitiveList;
}

VGList<VGPrimitive> const& VGPainter::getPrimitives() const
{
	return PRIVATE()->PrimitiveList;
}