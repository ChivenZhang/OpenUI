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
	UIBuilder(UIContextRaw context);
	virtual ~UIBuilder();
	UIContextRaw getContext() const;
	virtual bool addFactory(UIFactoryRef value);
	virtual bool removeFactory(UIString name);
	virtual void removeFactory();
	virtual UIElementRef buildElement(UIString html, UIString css = UIString()) const;

	template<class T, class... Args>
	UIRef<T> create(Args... args)
	{
		return UINew<T>(getContext(), std::forward<Args>(args)...);
	}

private:
	UIElementPrivateRaw m_Private;
};
using UIBuilderRaw = UIRaw<UIBuilder>;
using UIBuilderRef = UIRef<UIBuilder>;