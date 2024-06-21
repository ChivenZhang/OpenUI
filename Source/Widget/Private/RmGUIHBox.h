#pragma once
#include "RmGUILayout.h"

/// @brief Horizontal Layout
class RMGUI_API RmGUIHBox : public RmGUILayout
{
public:
	RmGUIHBox(IRmGUIWidgetRaw parent = nullptr);
	virtual void layout(RmRectRaw client) override;
	virtual void paint(IRmGUIPainterRaw painter, RmRectRaw client) override;
};