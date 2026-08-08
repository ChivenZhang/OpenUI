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
#include "UIFactory.h"

class OPENUI_API UIBuilder
{
public:
	UIBuilder(UICanvasRaw canvas);
	virtual ~UIBuilder();
	UICanvasRaw getCanvas() const;
	virtual bool addFactory(UIString tag, UIFactoryRef value);
	virtual bool removeFactory(UIString tag);
	virtual void removeFactory();
	virtual UIWidgetRef buildWidget(UIString html) const;

	template<class T, class... Args>
	UIRef<T> create(Args... args)
	{
		return UINew<T>(getCanvas(), std::forward<Args>(args)...);
	}

private:
	UIWidgetPrivateRaw m_Private;
};
using UIBuilderRaw = UIRaw<UIBuilder>;
using UIBuilderRef = UIRef<UIBuilder>;