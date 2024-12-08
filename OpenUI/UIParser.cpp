#include "UIParser.h"

UIRef<UIParser::XMLElement> UIParser::parse(UIString html, UIString css) const
{
	return nullptr;
}

UIRef<UIParser::XMLElement> UIParser::parseLayout(UIString html) const
{
	return UIRef<XMLElement>();
}

UIList<UIParser::CSSSelector> UIParser::parseStyle(UIString css) const
{
	return UIList<CSSSelector>();
}
