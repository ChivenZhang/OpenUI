#pragma once
#include "Widget/IRmGUIWidget.h"

class RmGUIWidget : public IRmGUIWidget
{
public:
	RmGUIWidget(rmwidget parent = nullptr);
	~RmGUIWidget();
	virtual rmwidget getParent() const override;
	virtual RmArrayView<RmRef<IRmGUIWidget>> getChildren() override;
	virtual RmArrayView<const RmRef<IRmGUIWidget>> getChildren() const override;
	virtual void paint(rmpainter painter) override;
	virtual void handle(rmevent_source source, rmevent event) override;

protected:
	rmwidget m_ParentWidget;
	RmVector<RmRef<IRmGUIWidget>> m_ChildWidgetList;
};