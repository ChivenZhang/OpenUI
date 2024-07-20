#include "UIParser.h"

UIRef<UIParser::XMLElement> UIParser::parse(UIString html, UIString css) const
{
	return nullptr;
}

UIRef<UIParser::XMLElement> UIParser::parseLayout(UIString html) const
{
	return UIRef<XMLElement>();
}

UIVector<UIParser::CSSSelector> UIParser::parseStyle(UIString css) const
{
	return UIVector<CSSSelector>();
}
