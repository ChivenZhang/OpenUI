#include "RmGUIContext.h"

RmRef<IRmGUIContext> IRmGUIContext::GetInstance()
{
	return RmNew<RmGUIContext>();
}

bool RmGUIContext::appendWidget(RmRef<IRmGUIWidget> widget)
{
	if (widget == nullptr) return false;
	auto result = std::find(m_TopLevelList.begin(), m_TopLevelList.end(), widget);
	if (result == m_TopLevelList.end()) m_TopLevelList.push_back(widget);
	return true;
}

bool RmGUIContext::removeWidget(RmRef<IRmGUIWidget> widget)
{
	auto result = std::remove(m_TopLevelList.begin(), m_TopLevelList.end(), widget);
	if (result == m_TopLevelList.end()) return false;
	m_TopLevelList.erase(result, m_TopLevelList.end());
	return true;
}

void RmGUIContext::sendEvent(rmreactor source, rmevent event)
{
	RmLambda<void(rmwidget)> foreach_func;
	foreach_func = [&](rmwidget widget) {
		if (widget->filter(source, event) == false) widget->handle(source, event);
		auto childrenList = widget->getChildren();
		for (size_t i = 0; i < childrenList.size(); ++i) foreach_func(childrenList[i].get());
		};
	for (size_t i = 0; i < m_TopLevelList.size(); ++i) foreach_func(m_TopLevelList[i].get());
}

void RmGUIContext::postEvent(rmreactor source, rmevent event)
{
}
