#pragma once
#include "RmGUIControl.h"

class RMGUI_API RmGUIPanel : public RmGUIControl
{
public:
	RmGUIPanel(rmwidget parent = nullptr);
	virtual void paint(rmpainter painter, rmrect client) override;
};