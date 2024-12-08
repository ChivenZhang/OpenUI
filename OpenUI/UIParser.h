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
#include "UI.h"

/// @brief 
class OPENUI_API UIParser
{
public:
	struct XMLStyle
	{
		UIString Name, Value;
	};
	struct XMLElement
	{
		UIString Name, Class, ID;
		UIList<XMLStyle> Style;
		// UIRaw<XMLElement> Parent;
		UIList<UIRef<XMLElement>> Children;
	};
	struct CSSSelector
	{
		UIString Name, Class, ID;
	};
	struct CSSDeclare
	{
		UIString Name, Value;
	};
	struct CSSElement
	{
		CSSSelector Selector;
		UIList<CSSDeclare> Declare;
	};

public:
	virtual ~UIParser() = default;
	virtual UIRef<XMLElement> parse(UIString html, UIString css) const;

protected:
	virtual UIRef<XMLElement> parseLayout(UIString html) const;
	virtual UIList<CSSSelector> parseStyle(UIString css) const;
};