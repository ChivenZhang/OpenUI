#pragma once
#include "RmGUILayout.h"

/// @brief Scroll Layout
class RMGUI_API RmGUIScroll : public RmGUILayout
{
public:
	RmGUIScroll(IRmGUIWidgetRaw parent = nullptr);
	virtual void layout(rmrect client) override;
	virtual void paint(IRmGUIPainterRaw painter, rmrect client) override;
};