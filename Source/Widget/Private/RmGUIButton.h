#pragma once
#include "RmGUIControl.h"
#include "RmGUILabel.h"

/// @brief Button Control
class RMGUI_API RmGUIButton : public RmGUIControl
{
public:
	RmGUIButton(IRmGUIWidgetRaw parent = nullptr);
	~RmGUIButton();
	virtual void layout(RmRectRaw client) override;
	virtual void paint(IRmGUIPainterRaw painter, RmRectRaw client) override;
	RmString getText() const;
	void setText(RmString const& text);
	RmGUILabelTextStyle getStyle() const;
	void setStyle(RmGUILabelTextStyle const& style);
	RmFontAligns getAlignment() const;
	void setAlignment(RmFontAligns value);
	bool getChecked() const;
	void  setChecked(bool value);
	bool getCheckable() const;
	void  setCheckable(bool value);

protected:
	virtual void mousePressEvent(IRmGUIMouseEventRaw event) override;
	virtual void mouseReleaseEvent(IRmGUIMouseEventRaw event) override;
	virtual void mouseMoveEvent(IRmGUIMouseEventRaw event) override;

public:
	IRmGUISignalAsRaw<bool> clicked;
	IRmGUISignalAsRaw<> pressed;
	IRmGUISignalAsRaw<> released;
	IRmGUISignalAsRaw<bool> toggled;

private:
	RmGUIWidgetPrivateRaw m_PrivateButton;
};
using RmGUIButtonRef = RmRef<RmGUIButton>;
using RmGUIButtonRaw = RmRaw<RmGUIButton>;