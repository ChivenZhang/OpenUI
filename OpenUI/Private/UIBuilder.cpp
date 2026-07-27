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

class UIBuilderPrivate : public UIWidgetPrivate
{
public:
	UICanvasRaw Canvas;
	UIStringMap<UIFactoryRef> FactoryMap;
};
#define PRIVATE() ((UIBuilderPrivate*) m_Private)

UIBuilder::UIBuilder(UICanvasRaw canvas)
{
	m_Private = new UIBuilderPrivate;

	PRIVATE()->Canvas = canvas;
}

UIBuilder::~UIBuilder()
{
	delete m_Private;
	m_Private = nullptr;
}

UICanvasRaw UIBuilder::getCanvas() const
{
	return PRIVATE()->Canvas;
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

UIWidgetRef UIBuilder::buildWidget(UIString html, UIString css) const
{
	UIParser parser;
	auto element = parser.parse(html, css);

	return UIWidgetRef();
}
