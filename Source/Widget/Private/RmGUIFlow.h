#pragma once
#include "RmGUILayout.h"

/// @brief Flow Layout
class RMGUI_API RmGUIFlow : public RmGUILayout
{
public:
	RmGUIFlow(IRmGUIWidgetRaw parent = nullptr);
	virtual void layout(RmRectRaw client) override;
	virtual void paint(IRmGUIPainterRaw painter, RmRectRaw client) override;
};