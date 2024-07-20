#include "UIBuilder.h"
#include "UIParser.h"

class UIBuilderPrivate : public UIElementPrivate
{
public:
	UIStringMap<UIFactoryRef> FactoryMap;
};
#define PRIVATE() ((UIBuilderPrivate*) m_Private)

UIBuilder::UIBuilder()
{
	m_Private = new UIBuilderPrivate;
}

UIBuilder::~UIBuilder()
{
	delete m_Private; m_Private = nullptr;
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