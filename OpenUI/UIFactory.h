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
	UIFactory(UIContextRaw context);

	virtual ~UIFactory() = default;

	virtual UIContextRaw getContext() const;

	virtual UIString getTagName() const = 0;

	virtual UIWidgetRef getElement(UIString style) const = 0;

protected:
	UIContextRaw m_Context;
};
using UIFactoryRef = UIRef<UIFactory>;
using UIFactoryRaw = UIRaw<UIFactory>;