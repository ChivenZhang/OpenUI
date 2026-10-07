#pragma once
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
#include "VGElement.h"
#include "VGRender.h"
#include "VGPainter.h"

/// @brief 
class OPENVG_API VGContext
{
public:
	VGContext();
	~VGContext();
	virtual VGPainterRaw getPainter() const;
	virtual void setPainter(VGPainterRef value);
	virtual VGRenderRaw getRender() const;
	virtual void setRender(VGRenderRef value);
	virtual void clipElement(VGElementRaw value);
	virtual void fillElement(VGElementRaw value);
	virtual void strokeElement(VGElementRaw value);
	virtual void renderElement(VGRect client);

private:
	VGPrivateRaw m_Private;
};
using VGContextRef = VGRef<VGContext>;
using VGContextRaw = VGRaw<VGContext>;