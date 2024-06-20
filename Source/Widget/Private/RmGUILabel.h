#pragma once
#include "RmGUIControl.h"

class RMGUI_API RmGUILabel : public RmGUIControl
{
public:
	RmGUILabel(rmwidget parent = nullptr);
	virtual void paint(rmpainter painter, rmrect client) override;
};