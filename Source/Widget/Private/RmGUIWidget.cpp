#include "RMGUIWidget.h"

RmGUIWidget::RmGUIWidget(rmwidget parent)
	:
	m_ParentWidget(parent)
{
}

RmGUIWidget::~RmGUIWidget()
{
}

RmRaw<IRmGUIWidget> RmGUIWidget::getParent() const
{
	return m_ParentWidget;
}

RmArrayView<RmRef<IRmGUIWidget>> RmGUIWidget::getChildren()
{
	return m_ChildWidgetList;
}

RmArrayView<const RmRef<IRmGUIWidget>> RmGUIWidget::getChildren() const
{
	return m_ChildWidgetList;
}

void RmGUIWidget::paint(rmpainter painter)
{
	// Do nothion.
}

void RmGUIWidget::handle(rmevent_source source, rmevent event)
{
	// Do nothion.
}