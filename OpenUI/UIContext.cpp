#include "UIContext.h"

struct UIContextElement
{
	UIElementRef Element;
	int32_t ZOrder;
};

class UIContextPrivateData : public UIContextPrivate
{
public:
	UIPainterRef Painter;
	UIRenderRef Render;
	UIElementRaw FocusElement;
	UIVector<UIContextElement> TopLevelList;
};
#define PRIVATE() ((UIContextPrivateData*) m_Private)

UIContext::UIContext()
{
	m_Private = new UIContextPrivateData;
}

UIContext::~UIContext()
{
	delete m_Private; m_Private = nullptr;
}

UIPainterRaw UIContext::getPainter() const
{
	return PRIVATE()->Painter.get();
}

void UIContext::setPainter(UIPainterRef value)
{
	PRIVATE()->Painter = value;
}

UIRenderRaw UIContext::getRender() const
{
	return PRIVATE()->Render.get();
}

void UIContext::setRender(UIRenderRef value)
{
	PRIVATE()->Render = value;
}

UIElementRaw UIContext::getFocus() const
{
	return PRIVATE()->FocusElement;
}

void UIContext::setFocus(UIElementRaw value)
{
	PRIVATE()->FocusElement = value;
}

bool UIContext::addElement(UIElementRef value, int32_t zorder)
{
	if (value == nullptr) return false;
	auto result = std::find_if(PRIVATE()->TopLevelList.begin(), PRIVATE()->TopLevelList.end(), [=](UIContextElement const& e)->bool { return e.Element == value; });
	if (result == PRIVATE()->TopLevelList.end()) PRIVATE()->TopLevelList.emplace_back(value, zorder);
	else result->ZOrder = zorder;
	value->setContext(this);
	value->setParent(nullptr);

	std::sort(PRIVATE()->TopLevelList.begin(), PRIVATE()->TopLevelList.end(), [](UIContextElement const& a, UIContextElement const& b)->bool {
		return a.ZOrder < b.ZOrder;
		});
	return true;
}

bool UIContext::removeElement(UIElementRef value)
{
	auto result = std::remove_if(PRIVATE()->TopLevelList.begin(), PRIVATE()->TopLevelList.end(), [=](UIContextElement const& e)->bool { return e.Element == value; });
	if (result == PRIVATE()->TopLevelList.end()) return false;
	PRIVATE()->TopLevelList.erase(result, PRIVATE()->TopLevelList.end());
	value->setContext(nullptr);
	value->setParent(nullptr);
	return true;
}

void UIContext::removeElement()
{
}

bool UIContext::sendEvent(UIReactorRaw sender, UIEventRaw event)
{
	return false;
}

void UIContext::postEvent(UIReactorRef sender, UIEventRef event)
{
}

void UIContext::layoutElement(UIRect client)
{
}

void UIContext::paintElement(UIRect client)
{
}

void UIContext::renderElement(UIRect client)
{
}