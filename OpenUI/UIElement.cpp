#include "UIElement.h"
#include "UIPainter.h"

class UIElementPrivateData : public UIElementPrivate
{
public:
	UIElementRaw Parent = nullptr;
	UIVector<UIElementRef> Children;
	UIPainterRef Painter;
	UIContextRaw Context = nullptr;
	UIPointUV3 Primitive[2];
};
#define PRIVATE() ((UIElementPrivateData*) m_Private)

UIElement::UIElement()
{
	m_Private = new UIElementPrivateData;
}

UIElement::~UIElement()
{
	delete m_Private; m_Private = nullptr;
}

UIElementRaw UIElement::getParent() const
{
	return PRIVATE()->Parent;
}

UIArrayView<const UIElementRef> UIElement::getChildren() const
{
	return PRIVATE()->Children;
}

UIString UIElement::getStyle() const
{
	return UIString();
}

void UIElement::setStyle(UIString value)
{
}

UIString UIElement::getStyle(UIString name) const
{
	return UIString();
}

void UIElement::setStyle(UIString name, UIString value)
{
}

bool UIElement::addElement(UIElementRef value)
{
	if (value == nullptr) return false;
	auto result = std::find(PRIVATE()->Children.begin(), PRIVATE()->Children.end(), value);
	if (result == PRIVATE()->Children.end()) PRIVATE()->Children.push_back(value);
	value->setContext(PRIVATE()->Context);
	value->setParent(this);
	return true;
}

bool UIElement::removeElement(UIElementRef value)
{
	auto result = std::remove(PRIVATE()->Children.begin(), PRIVATE()->Children.end(), value);
	if (result == PRIVATE()->Children.end()) return false;
	PRIVATE()->Children.erase(result, PRIVATE()->Children.end());
	value->setContext(nullptr);
	value->setParent(nullptr);
	return true;
}

void UIElement::removeElement()
{
}

void UIElement::layout(UIRect client)
{
}

void UIElement::paint(UIRect client, UIPainterRaw painter)
{
}

UIContextRaw UIElement::getContext() const
{
	return PRIVATE()->Context;
}

void UIElement::setContext(UIContextRaw value)
{
	PRIVATE()->Context = value;
}

void UIElement::setParent(UIElementRaw value)
{
	if (value == this) return;
	PRIVATE()->Parent = value;
}