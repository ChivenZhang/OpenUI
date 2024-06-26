#pragma once
#include "RmGUILayout.h"

/// @brief Layered Layout
class RMGUI_API RmGUILayer : public RmGUILayout
{
public:
	RmGUILayer(IRmGUIWidgetRaw parent = nullptr);
	virtual void layout(RmRectRaw client) override;
	virtual void paint(IRmGUIPainterRaw painter, RmRectRaw client) override;
};