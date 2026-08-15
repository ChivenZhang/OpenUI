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
	virtual bool setFactory(UIString tag, UIFactoryRef value);
	virtual bool unsetFactory(UIString tag);
	virtual void unsetFactory();
	virtual UIWidgetRef buildWidget(UIString html) const;

	template<class T, class... Args>
	UIRef<T> newWidget(Args... args)
	{
		return UINew<T>(getCanvas(), std::forward<Args>(args)...);
	}

private:
	UIPrivateRaw m_Private;
};
using UIBuilderRaw = UIRaw<UIBuilder>;
using UIBuilderRef = UIRef<UIBuilder>;