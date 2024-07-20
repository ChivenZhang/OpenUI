#pragma once
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
		UIVector<XMLStyle> Style;
		// UIRaw<XMLElement> Parent;
		UIVector<UIRef<XMLElement>> Children;
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
		UIVector<CSSDeclare> Declare;
	};

public:
	virtual ~UIParser() = default;
	virtual UIRef<XMLElement> parse(UIString html, UIString css) const;

protected:
	virtual UIRef<XMLElement> parseLayout(UIString html) const;
	virtual UIVector<CSSSelector> parseStyle(UIString css) const;
};