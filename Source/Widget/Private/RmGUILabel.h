#pragma once
#include "RmGUIWidget.h"

class RMGUI_API RmGUILabel : public RmGUIWidget
{
public:
	RmGUILabel(rmwidget parent = nullptr);
	virtual void paint(rmpainter painter, rmrect client) override;

protected:
	virtual void mouseMoveEvent(rmevent_mouse event) override;
};