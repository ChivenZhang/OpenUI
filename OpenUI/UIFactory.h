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

	virtual UICanvasRaw getContext() const;

	virtual UIString getTagName() const = 0;

	virtual UIWidgetRef newWidget(UIString style) const = 0;

protected:
	UICanvasRaw m_Context;
};
using UIFactoryRef = UIRef<UIFactory>;
using UIFactoryRaw = UIRaw<UIFactory>;