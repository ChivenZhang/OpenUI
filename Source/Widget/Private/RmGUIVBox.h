#pragma once
#include "RmGUILayout.h"

/// @brief Vertical Layout
class RMGUI_API RmGUIVBox : public RmGUILayout
{
public:
	RmGUIVBox(IRmGUIWidgetRaw parent = nullptr);
	virtual void layout(rmrect client) override;
};