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
#include "UIStyle.h"
class UICanvas;
using UICanvasRaw = UIRaw<UICanvas>;
class UIRenderPrivate {};
using UIRenderPrivateRaw = UIRaw<UIRenderPrivate>;

/// @brief Base interface of render backend.
class OPENUI_API UIRender
{
public:
	virtual ~UIRender() = default;

	virtual UIString getName() const = 0;

	virtual UICanvasRaw getCanvas() const = 0;

	virtual void render(UIRect client, UIImageRaw srcImage, UIImageRaw dstImage, UIComputedStyleRaw style) = 0;
};
using UIRenderRef = UIRef<UIRender>;
using UIRenderRaw = UIRaw<UIRender>;