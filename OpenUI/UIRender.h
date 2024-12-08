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
#include "UI.h"

/// @brief 
class UIRenderPrivate {};
using UIRenderPrivateRaw = UIRaw<UIRenderPrivate>;

/// @brief Base interface of render backend.
class OPENUI_API UIRender
{
public:
	virtual ~UIRender() = default;

	virtual void render(UIRect client, UIArrayView<UIPrimitive> data) = 0;
};
using UIRenderRef = UIRef<UIRender>;
using UIRenderRaw = UIRaw<UIRender>;