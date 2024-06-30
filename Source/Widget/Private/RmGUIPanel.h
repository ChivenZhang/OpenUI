#pragma once
#include "RmGUIControl.h"

class RMGUI_API RmGUIPanel : public RmGUIControl
{
public:
	RmGUIPanel(IRmGUIWidgetRaw parent = nullptr);
	~RmGUIPanel();
	virtual void paint(IRmGUIPainterRaw painter, RmRectRaw client) override;

private:
	RmGUIWidgetPrivateRaw m_PrivatePanel;
};