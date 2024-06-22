#pragma once
#include "RmGUIControl.h"

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

private:
	RmGUIWidgetPrivate* m_PrivateButton;
};