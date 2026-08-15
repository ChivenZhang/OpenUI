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
class VGContext;

/// @brief 
class OPENVG_API VGPainter
{
public:
	VGPainter();
	~VGPainter();

	void clip(VGElementRaw element);
	void fill(VGElementRaw element);
	void stroke(VGElementRaw element);
	VGList<VGPrimitive> const& getPrimitives() const;

private:
	VGList<VGPrimitive>& getPrimitiveList();

private:
	VGPrivateRaw m_Private;

	friend class VGContext;
};
using VGPainterRef = VGRef<VGPainter>;
using VGPainterRaw = VGRaw<VGPainter>;