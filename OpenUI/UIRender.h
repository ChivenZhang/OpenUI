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
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
class UICanvas;
using UICanvasRaw = UIRaw<UICanvas>;
class UIRenderPrivate {};
using UIRenderPrivateRaw = UIRaw<UIRenderPrivate>;

using UIMat4 = glm::mat4;

/// @brief Base interface of render backend.
class OPENUI_API UIRender
{
public:
	virtual ~UIRender() = default;

	virtual UIString getName() const = 0;

	virtual UICanvasRaw getCanvas() const = 0;

	virtual void render(UIRect client, UIMat4 matrix, UIImageRaw srcImg, UIImageRaw dstImg, UIComputedStyleRaw style) = 0;

	virtual UIImage newImage(uint32_t width, uint32_t height) = 0;

	virtual void delImage(UIImage value) = 0;
};
using UIRenderRef = UIRef<UIRender>;
using UIRenderRaw = UIRaw<UIRender>;