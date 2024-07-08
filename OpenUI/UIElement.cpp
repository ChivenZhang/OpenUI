#include "UIElement.h"

class UIElementPrivateData : public UIElementPrivate
{
public:
	UIElementRaw Parent = nullptr;
	UIVector<UIElementRef> Children;
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

bool UIElement::addElement(UIElementRef value)
{
	return false;
}

bool UIElement::removeElement(UIElementRef value)
{
	return false;
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

void UIElement::layout(UIRect client)
{
}

void UIElement::paint(UIRect client, UIPainterRaw painter)
{
}
