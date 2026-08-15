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
#include "../VGContext.h"

struct VGContextPrivate : VGPrivate
{
	VGRenderRef Render;
	VGPainterRef Painter;
};
#define PRIVATE() ((VGContextPrivate*)m_Private)

VGContext::VGContext()
{
	m_Private = new VGContextPrivate;
}

VGContext::~VGContext()
{
	delete m_Private; m_Private = nullptr;
}

VGPainterRaw VGContext::getPainter() const
{
	return PRIVATE()->Painter.get();
}

void VGContext::setPainter(VGPainterRef value)
{
	PRIVATE()->Painter = value;
}

VGRenderRaw VGContext::getRender() const
{
	return PRIVATE()->Render.get();
}

void VGContext::setRender(VGRenderRef value)
{
	PRIVATE()->Render = value;
}

void VGContext::clipElement(VGElementRaw value)
{
	if (PRIVATE()->Render == nullptr) return;
	PRIVATE()->Painter->clip(value);
}

void VGContext::fillElement(VGElementRaw value)
{
	if (PRIVATE()->Render == nullptr) return;
	PRIVATE()->Painter->fill(value);
}

void VGContext::strokeElement(VGElementRaw value)
{
	if (PRIVATE()->Render == nullptr) return;
	PRIVATE()->Painter->stroke(value);
}

void VGContext::renderElement(VGRect client)
{
	if (getPainter() && getRender())
	{
		getRender()->render(client, getPainter()->getPrimitiveList());
		getPainter()->getPrimitiveList().clear();
	}
}
