#pragma once
#include "RmGUILayout.h"

class RMGUI_API RmGUIHBox : public RmGUILayout
{
public:
	RmGUIHBox(IRmGUIWidgetRaw parent);
	virtual void layout(rmrect client) override;
};