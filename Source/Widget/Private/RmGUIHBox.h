#pragma once
#include "RmGUILayout.h"

class RMGUI_API RmGUIHBox : public RmGUILayout
{
public:
	RmGUIHBox(rmwidget parent);
	virtual void layout(rmrect client) override;
};