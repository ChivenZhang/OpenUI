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
	explicit UIFactory(UICanvasRaw canvas);

	virtual ~UIFactory() = default;

	virtual UICanvasRaw getCanvas() const;

	virtual UIWidgetRef newWidget() const = 0;

protected:
	UICanvasRaw m_Context;
};
using UIFactoryRef = UIRef<UIFactory>;
using UIFactoryRaw = UIRaw<UIFactory>;

template<class T>
class UIWidgetFactory : public UIFactory
{
public:
	using UIFactory::UIFactory;

	UIWidgetRef newWidget() const override
	{
		auto result = UINew<T>(getCanvas());
		return result;
	}
};