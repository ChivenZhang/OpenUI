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
#include "VG.h"

/// @brief 
class OPENVG_API VGRender
{
public:
	virtual ~VGRender() = default;

	virtual VGImageRaw getTarget() const = 0;

	virtual void setTarget(VGImageRaw value) = 0;

	virtual void render(VGRect client, VGListView<const VGPrimitive> data) = 0;
};
using VGRenderRef = VGRef<VGRender>;
using VGRenderRaw = VGRaw<VGRender>;