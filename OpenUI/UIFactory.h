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
#include "UIWidget.h"

/// @brief Base interface of factories
class OPENUI_API UIFactory
{
public:
	virtual ~UIFactory() = default;
	virtual UIWidgetRef newWidget(UICanvasRaw canvas) const = 0;
};
using UIFactoryRef = UIRef<UIFactory>;
using UIFactoryRaw = UIRaw<UIFactory>;

template<class T>
class UIWidgetFactory : public UIFactory
{
public:
	UIWidgetRef newWidget(UICanvasRaw canvas) const override
	{
		auto result = UINew<T>(canvas);
		return result;
	}
};