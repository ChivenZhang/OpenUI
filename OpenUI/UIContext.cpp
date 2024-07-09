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
	UIElementRaw Focus;
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
	return PRIVATE()->Focus;
}

void UIContext::setFocus(UIElementRaw value)
{
	PRIVATE()->Focus = value;
}

void UIContext::sendEvent(UIReactorRaw sender, UIEventRaw event)
{
	UILambda<void(UIElementRaw)> foreach_func;
	foreach_func = [&](UIElementRaw element) {
		if (element->getVisible() == false) return;
		if (element->getEventFilter() && element->getEventFilter()->filter(sender, event) == true) return;
		if (element->filter(sender, event) == true) return;
		auto childList = element->getChildren();
		for (size_t i = 0; i < childList.size(); ++i) foreach_func(childList[i].get());
		if (event->Accept == false) element->handle(sender, event);
		};
	for (size_t i = 0; i < PRIVATE()->TopLevelList.size(); ++i)
	{
		auto element = PRIVATE()->TopLevelList[PRIVATE()->TopLevelList.size() - 1 - i].Element.get();
		if (element->getVisible() == false) continue;
		foreach_func(element);
		break;
	}
}

void UIContext::postEvent(UIReactorRef sender, UIEventRef event)
{
}

bool UIContext::addElement(UIElementRef value, int32_t zorder)
{
	if (value == nullptr) return false;
	auto result = std::find_if(PRIVATE()->TopLevelList.begin(), PRIVATE()->TopLevelList.end(), [=](UIContextElement const& e)->bool { return e.Element == value; });
	if (result == PRIVATE()->TopLevelList.end()) PRIVATE()->TopLevelList.emplace_back(value, zorder);
	else result->ZOrder = zorder;
	value->setContext(this);
	value->setParent(nullptr);
	std::sort(PRIVATE()->TopLevelList.begin(), PRIVATE()->TopLevelList.end(), [](UIContextElement const& a, UIContextElement const& b) { return a.ZOrder < b.ZOrder; });
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
	for (size_t i = 0; i < PRIVATE()->TopLevelList.size(); ++i)
	{
		PRIVATE()->TopLevelList[i].Element->setContext(nullptr);
		PRIVATE()->TopLevelList[i].Element->setParent(nullptr);
	}
	PRIVATE()->TopLevelList.clear();
}

void UIContext::layoutElement(UIRect client)
{
	UILambda<void(UIElementRaw, UIRect client)> foreach_func;
	foreach_func = [&](UIElementRaw element, UIRect client) {
		element->layout(client);
		auto childList = element->getChildren();
		for (size_t i = 0; i < childList.size(); ++i) foreach_func(childList[i].get(), childList[i]->getBounds());
		};
	for (size_t i = 0; i < PRIVATE()->TopLevelList.size(); ++i)
	{
		PRIVATE()->TopLevelList[i].Element->setBounds(client);
		PRIVATE()->TopLevelList[i].Element->setViewport(client);
		foreach_func(PRIVATE()->TopLevelList[i].Element.get(), PRIVATE()->TopLevelList[i].Element->getBounds());
	}
}

void UIContext::paintElement(UIRect client)
{
	UILambda<void(UIElementRaw, UIRect, UIPainterRaw)> foreach_func;
	foreach_func = [&](UIElementRaw element, UIRect client, UIPainterRaw painter) {
		if (element->getVisible() == false) return;
		if (element->getPainter()) painter = element->getPainter();
		if (painter == nullptr) return;
		element->paint(client, painter);
		auto childList = element->getChildren();
		for (size_t i = 0; i < childList.size(); ++i) foreach_func(childList[i].get(), childList[i]->getBounds(), painter);
		};
	for (size_t i = 0; i < PRIVATE()->TopLevelList.size(); ++i)
	{
		foreach_func(PRIVATE()->TopLevelList[i].Element.get(), PRIVATE()->TopLevelList[i].Element->getBounds(), getPainter());
	}
}

void UIContext::renderElement(UIRect client)
{
	if (getRender() == nullptr) return;

	UIVector<UIPrimitive> renderList;
	UILambda<void(UIElementRaw, UIRect client)> foreach_func;
	foreach_func = [&](UIElementRaw element, UIRect client) {
		if (element->getVisible() == false) return;
		if (element->getPainter()) renderList.emplace_back(element->getPainter(), element->getPrimitive());
		auto childList = element->getChildren();
		for (size_t i = 0; i < childList.size(); ++i) foreach_func(childList[i].get(), childList[i]->getBounds());
		};

	if (getPainter())
	{
		auto viewport = client;
		UIPointUV3 primitive[2];
		primitive[0].P0 = { viewport.X, viewport.Y };
		primitive[0].P1 = { viewport.X + viewport.W, viewport.Y };
		primitive[0].P2 = { viewport.X + viewport.W, viewport.Y + viewport.H };
		primitive[1].P0 = { viewport.X, viewport.Y };
		primitive[1].P1 = { viewport.X + viewport.W, viewport.Y + viewport.H };
		primitive[1].P2 = { viewport.X, viewport.Y + viewport.H };
		renderList.emplace_back(getPainter(), primitive);
	}

	for (size_t i = 0; i < PRIVATE()->TopLevelList.size(); ++i)
	{
		foreach_func(PRIVATE()->TopLevelList[i].Element.get(), PRIVATE()->TopLevelList[i].Element->getBounds());
	}
	getRender()->render(client, renderList);
}