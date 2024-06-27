#include "RmGUIContext.h"
#include "RmGUIWidget.h"

RmRef<IRmGUIContext> IRmGUIContext::GetInstance()
{
	return RmNew<RmGUIContext>();
}

IRmGUISurfaceRaw RmGUIContext::getSurface() const
{
	return m_Surface.get();
}

void RmGUIContext::setSurface(IRmGUISurfaceRef value)
{
	m_Surface = value;
}

bool RmGUIContext::addWidget(IRmGUIWidgetRef value, int32_t zorder)
{
	auto widget = RmCast<RmGUIWidget>(value);
	if (widget == nullptr) return false;
	auto result = std::find_if(m_TopLevelList.begin(), m_TopLevelList.end(), [=](widget_t const& a)->bool { return a.Widget == widget; });
	if (result == m_TopLevelList.end()) m_TopLevelList.push_back({ .Widget = value, .ZOrder = zorder, });
	else result->ZOrder = zorder;
	widget->setContext(this);

	std::sort(m_TopLevelList.begin(), m_TopLevelList.end(), [](widget_t const& a, widget_t const& b)->bool {
		return a.ZOrder < b.ZOrder;
		});
	return true;
}

bool RmGUIContext::removeWidget(IRmGUIWidgetRef value)
{
	auto widget = RmCast<RmGUIWidget>(value);
	auto result = std::remove_if(m_TopLevelList.begin(), m_TopLevelList.end(), [=](widget_t const& a)->bool { return a.Widget == widget; });
	if (result == m_TopLevelList.end()) return false;
	m_TopLevelList.erase(result, m_TopLevelList.end());
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
	for (size_t i = 0; i < m_TopLevelList.size(); ++i)
	{
		auto widget = m_TopLevelList[m_TopLevelList.size() - 1 - i].Widget.get();
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
	for (size_t i = 0; i < m_TopLevelList.size(); ++i)
	{
		RmRect viewport;
		if (std::isnan(m_TopLevelList[i].Widget->getFixedWidth()))
		{
			viewport.X = 0;
			viewport.W = client.W;
		}
		else
		{
			viewport.X = m_TopLevelList[i].Widget->getPositionX();
			viewport.W = m_TopLevelList[i].Widget->getFixedWidth();
		}
		if (std::isnan(m_TopLevelList[i].Widget->getFixedHeight()))
		{
			viewport.Y = 0;
			viewport.H = client.H;
		}
		else
		{
			viewport.Y = m_TopLevelList[i].Widget->getPositionY();
			viewport.H = m_TopLevelList[i].Widget->getFixedHeight();
		}
		m_TopLevelList[i].Widget->setRect(viewport);
		m_TopLevelList[i].Widget->setViewport(viewport);
		foreach_func(m_TopLevelList[i].Widget.get(), m_TopLevelList[i].Widget->getRect());
	}
}

void RmGUIContext::paintWidget(RmRect client)
{
	if (m_Surface == nullptr || m_Surface->getPainter() == nullptr) return;
	RmLambda<void(IRmGUIWidgetRaw, RmRect client)> foreach_func;
	foreach_func = [&](IRmGUIWidgetRaw widget, RmRect client) {
		if (widget->getVisible() == false) return;
		widget->paint(m_Surface->getPainter(), &client);
		auto childList = widget->getChildren();
		for (size_t i = 0; i < childList.size(); ++i) foreach_func(childList[i].get(), childList[i]->getRect());
		};
	for (size_t i = 0; i < m_TopLevelList.size(); ++i)
	{
		foreach_func(m_TopLevelList[i].Widget.get(), m_TopLevelList[i].Widget->getRect());
	}
}
