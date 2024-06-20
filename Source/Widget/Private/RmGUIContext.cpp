#include "RmGUIContext.h"
#include "RmGUIWidget.h"

RmRef<IRmGUIContext> IRmGUIContext::GetInstance()
{
	return RmNew<RmGUIContext>();
}

bool RmGUIContext::addWidget(RmRef<IRmGUIWidget> value)
{
	auto widget = RmCast<RmGUIWidget>(value);
	if (widget == nullptr) return false;
	auto result = std::find(m_TopLevelList.begin(), m_TopLevelList.end(), widget);
	if (result == m_TopLevelList.end()) m_TopLevelList.push_back(widget);
	widget->setContext(this);
	return true;
}

bool RmGUIContext::removeWidget(RmRef<IRmGUIWidget> value)
{
	auto widget = RmCast<RmGUIWidget>(value);
	auto result = std::remove(m_TopLevelList.begin(), m_TopLevelList.end(), widget);
	if (result == m_TopLevelList.end()) return false;
	m_TopLevelList.erase(result, m_TopLevelList.end());
	widget->setContext(nullptr);
	return true;
}

void RmGUIContext::sendEvent(rmreactor source, rmevent event)
{
	RmLambda<void(rmwidget)> foreach_func;
	foreach_func = [&](rmwidget widget) {
		auto result = widget->filter(source, event);
		auto childrenList = widget->getChildren();
		for (size_t i = 0; i < childrenList.size(); ++i) foreach_func(childrenList[i].get());
		if (result == false) widget->handle(source, event);
		};
	for (size_t i = 0; i < m_TopLevelList.size(); ++i) foreach_func(m_TopLevelList[i].get());
}

void RmGUIContext::postEvent(rmreactor source, rmevent event)
{
}

bool RmGUIContext::renderWidget(RmRaw<IRmGUIPainter> painter, RmRect client)
{
	for (size_t i = 0; i < m_TopLevelList.size(); ++i)
	{
		m_TopLevelList[i]->paint(painter, &client);
	}
	return true;
}
