#pragma once
#include "RmGUIControl.h"

class RMGUI_API RmGUIPanel : public RmGUIControl
{
public:
	RmGUIPanel(IRmGUIWidgetRaw parent = nullptr);
	virtual void paint(rmpainter painter, rmrect client) override;
};