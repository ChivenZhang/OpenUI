#pragma once
#include "RmGUIControl.h"
#include "RmGUISignal.h"

/// @brief Button Control
class RMGUI_API RmGUIButton : public RmGUIControl
{
public:
	RmGUIButton(IRmGUIWidgetRaw parent = nullptr);
	~RmGUIButton();
	virtual void paint(IRmGUIPainterRaw painter, RmRectRaw client) override;

protected:
	virtual void mousePressEvent(IRmGUIMouseEventRaw event) override;
	virtual void mouseReleaseEvent(IRmGUIMouseEventRaw event) override;
	virtual void mouseMoveEvent(IRmGUIMouseEventRaw event) override;

public:
	IRmGUISignalAsRaw<> onClicked;

private:
	RmGUIWidgetPrivateRaw m_PrivateButton;
};