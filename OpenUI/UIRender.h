#pragma once
#include "UI.h"

/// @brief 
class UIRenderPrivate
{
public:
	virtual ~UIRenderPrivate() = default;
};
using UIRenderPrivateRaw = UIRaw<UIRenderPrivate>;

/// @brief Base interface of render backend.
class OPENUI_API UIRender
{
public:
	virtual ~UIRender() = default;

	virtual void render(UIRect client, UIArrayView<UIPrimitive> widgets) = 0;
};
using UIRenderRef = UIRef<UIRender>;
using UIRenderRaw = UIRaw<UIRender>;