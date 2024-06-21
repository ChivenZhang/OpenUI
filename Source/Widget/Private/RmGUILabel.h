#pragma once
#include "RmGUIControl.h"

class RMGUI_API RmGUILabel : public RmGUIControl
{
public:
	RmGUILabel(IRmGUIWidgetRaw parent = nullptr);
	virtual void paint(rmpainter painter, rmrect client) override;
};