#pragma once
#include "Widget/Private/RmGUIWidget.h"

class RMGUI_API RmGUIPanel : public RmGUIWidget
{
public:
	RmGUIPanel(rmwidget parent = nullptr);
	virtual void paint(rmpainter painter, rmrect client) override;

protected:
	virtual void mouseMoveEvent(rmevent_mouse event) override;
};