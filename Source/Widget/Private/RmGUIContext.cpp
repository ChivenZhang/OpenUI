#include "RmGUIContext.h"
#include "RmGUIWidget.h"

struct RmGUIContextWidget
{
	RmRef<IRmGUIWidget> Widget;
	int32_t ZOrder;
};

class RmGUIContextPrivateData : public RmGUIContextPrivate
{
public:
	RmRef<IRmGUIPainter> Painter;
	RmRef<IRmGUIRender> Render;
	RmRef<IRmGUIWidget> FocusWidget;
	RmVector<RmGUIContextWidget> TopLevelList;
};
#define PRIVATE() ((RmGUIContextPrivateData*)m_Private)

RmGUIContext::RmGUIContext()
	:
	m_Private(nullptr)
{
	m_Private = new RmGUIContextPrivateData;
}

RmGUIContext::~RmGUIContext()
{
	delete m_Private; m_Private = nullptr;
}

IRmGUIPainterRaw RmGUIContext::getPainter() const
{
	return PRIVATE()->Painter.get();
}

void RmGUIContext::setPainter(IRmGUIPainterRef value)
{
	PRIVATE()->Painter = value;
}

IRmGUIRenderRaw RmGUIContext::getRender() const
{
	return PRIVATE()->Render.get();
}

void RmGUIContext::setRender(IRmGUIRenderRef value)
{
	PRIVATE()->Render = value;
}

bool RmGUIContext::addWidget(IRmGUIWidgetRef value, int32_t zorder)
{
	auto widget = RmCast<RmGUIWidget>(value);
	if (widget == nullptr) return false;
	auto result = std::find_if(PRIVATE()->TopLevelList.begin(), PRIVATE()->TopLevelList.end(), [=](RmGUIContextWidget const& a)->bool { return a.Widget == widget; });
	if (result == PRIVATE()->TopLevelList.end()) PRIVATE()->TopLevelList.push_back({ .Widget = value, .ZOrder = zorder, });
	else result->ZOrder = zorder;
	widget->setContext(this);

	std::sort(PRIVATE()->TopLevelList.begin(), PRIVATE()->TopLevelList.end(), [](RmGUIContextWidget const& a, RmGUIContextWidget const& b)->bool {
		return a.ZOrder < b.ZOrder;
		});
	return true;
}

bool RmGUIContext::removeWidget(IRmGUIWidgetRef value)
{
	auto widget = RmCast<RmGUIWidget>(value);
	auto result = std::remove_if(PRIVATE()->TopLevelList.begin(), PRIVATE()->TopLevelList.end(), [=](RmGUIContextWidget const& a)->bool { return a.Widget == widget; });
	if (result == PRIVATE()->TopLevelList.end()) return false;
	PRIVATE()->TopLevelList.erase(result, PRIVATE()->TopLevelList.end());
	widget->setContext(nullptr);
	return true;
}

void RmGUIContext::sendEvent(IRmGUIReactorRaw source, IRmGUIEventRaw event)
{
	RmLambda<void(IRmGUIWidgetRaw)> foreach_func;
	foreach_func = [&](IRmGUIWidgetRaw widget) {
		if (widget->getVisible() == false) return;
		if (widget->getEventFilter() && widget->getEventFilter()->filter(source, event) == true) return;
		if (widget->filter(source, event) == true) return;
		auto childList = widget->getChildren();
		for (size_t i = 0; i < childList.size(); ++i) foreach_func(childList[i].get());
		if (event->Accept == false) widget->handle(source, event);
		};
	for (size_t i = 0; i < PRIVATE()->TopLevelList.size(); ++i)
	{
		auto widget = PRIVATE()->TopLevelList[PRIVATE()->TopLevelList.size() - 1 - i].Widget.get();
		if (widget->getVisible() == false) continue;
		foreach_func(widget);
		break;
	}
}

void RmGUIContext::postEvent(IRmGUIReactorRaw source, IRmGUIEventRaw event)
{
}

void RmGUIContext::layoutWidget(RmRect client)
{
	RmLambda<void(IRmGUIWidgetRaw, RmRect client)> foreach_func;
	foreach_func = [&](IRmGUIWidgetRaw widget, RmRect client) {
		widget->layout(&client);
		auto childList = widget->getChildren();
		for (size_t i = 0; i < childList.size(); ++i) foreach_func(childList[i].get(), childList[i]->getRect());
		};
	for (size_t i = 0; i < PRIVATE()->TopLevelList.size(); ++i)
	{
		RmRect viewport;
		if (std::isnan(PRIVATE()->TopLevelList[i].Widget->getFixedWidth()))
		{
			viewport.X = 0;
			viewport.W = client.W;
		}
		else
		{
			viewport.X = PRIVATE()->TopLevelList[i].Widget->getPositionX();
			viewport.W = PRIVATE()->TopLevelList[i].Widget->getFixedWidth();
		}
		if (std::isnan(PRIVATE()->TopLevelList[i].Widget->getFixedHeight()))
		{
			viewport.Y = 0;
			viewport.H = client.H;
		}
		else
		{
			viewport.Y = PRIVATE()->TopLevelList[i].Widget->getPositionY();
			viewport.H = PRIVATE()->TopLevelList[i].Widget->getFixedHeight();
		}
		PRIVATE()->TopLevelList[i].Widget->setRect(viewport);
		PRIVATE()->TopLevelList[i].Widget->setViewport(viewport);
		foreach_func(PRIVATE()->TopLevelList[i].Widget.get(), PRIVATE()->TopLevelList[i].Widget->getRect());
	}
}

void RmGUIContext::paintWidget(RmRect client)
{
	if (getPainter() == nullptr) return;

	RmLambda<void(IRmGUIWidgetRaw, RmRect client)> foreach_func;
	foreach_func = [&](IRmGUIWidgetRaw widget, RmRect client) {
		if (widget->getVisible() == false) return;
		if (widget->getPainter()) widget->paint(widget->getPainter(), &client);
		else widget->paint(getPainter(), &client);
		auto childList = widget->getChildren();
		for (size_t i = 0; i < childList.size(); ++i) foreach_func(childList[i].get(), childList[i]->getRect());
		};
	for (size_t i = 0; i < PRIVATE()->TopLevelList.size(); ++i)
	{
		foreach_func(PRIVATE()->TopLevelList[i].Widget.get(), PRIVATE()->TopLevelList[i].Widget->getRect());
	}
}

void RmGUIContext::renderWidget(RmRect client)
{
	if (getRender() == nullptr) return;

	RmVector<RmPrimitive> renderList;
	RmLambda<void(IRmGUIWidgetRaw, RmRect client)> foreach_func;
	foreach_func = [&](IRmGUIWidgetRaw widget, RmRect client) {
		if (widget->getVisible() == false) return;
		if (widget->getPainter())
		{
			renderList.emplace_back(RmPrimitive{ widget->getPainter(), widget->getPrimitive() });
		}
		auto childList = widget->getChildren();
		for (size_t i = 0; i < childList.size(); ++i) foreach_func(childList[i].get(), childList[i]->getRect());
		};

	if (getPainter())
	{
		RmPointUV3 primitive[2];
		primitive[0].P0 = { client.X, client.Y, 0, 0 };
		primitive[0].P1 = { client.X + client.W, client.Y, 0, 0 };
		primitive[0].P2 = { client.X + client.W, client.Y + client.H, 0, 0 };
		primitive[1].P0 = { client.X, client.Y, 0, 0 };
		primitive[1].P1 = { client.X + client.W, client.Y + client.H, 0, 0 };
		primitive[1].P2 = { client.X, client.Y + client.H, 0, 0 };
		renderList.emplace_back(RmPrimitive{ getPainter(), primitive });
	}

	for (size_t i = 0; i < PRIVATE()->TopLevelList.size(); ++i)
	{
		foreach_func(PRIVATE()->TopLevelList[i].Widget.get(), PRIVATE()->TopLevelList[i].Widget->getRect());
	}
	getRender()->render(renderList);
}