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
#include "../UIBuilder.h"
#include "../UIParser.h"

class UIBuilderPrivate : public UIElementPrivate
{
public:
	UIContextRaw Context;
	UIStringMap<UIFactoryRef> FactoryMap;
};
#define PRIVATE() ((UIBuilderPrivate*) m_Private)

UIBuilder::UIBuilder(UIContextRaw context)
{
	m_Private = new UIBuilderPrivate;

	PRIVATE()->Context = context;
}

UIBuilder::~UIBuilder()
{
	delete m_Private;
	m_Private = nullptr;
}

UIContextRaw UIBuilder::getContext() const
{
	return PRIVATE()->Context;
}

bool UIBuilder::addFactory(UIFactoryRef value)
{
	if (value == nullptr) return false;
	PRIVATE()->FactoryMap[value->getTagName()] = value;
	return true;
}

bool UIBuilder::removeFactory(UIString name)
{
	return PRIVATE()->FactoryMap.erase(name);
}

void UIBuilder::removeFactory()
{
	PRIVATE()->FactoryMap.clear();
}

UIElementRef UIBuilder::buildElement(UIString html, UIString css) const
{
	UIParser parser;
	auto element = parser.parse(html, css);

	return UIElementRef();
}
