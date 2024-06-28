#pragma once
#include "RmGUIControl.h"
#include "RmGUILabel.h"

struct RmGUIButtonStyle
{
	struct Mode
	{
		RmPen Pen;
		RmBrush Brush;
	};
	RmFloat2 Round;
	Mode Normal, Hover, Press, Disable;
};

/// @brief Slider Control
class RMGUI_API RmGUIButton : public RmGUIControl
{
public:
	RmGUIButton(IRmGUIWidgetRaw parent = nullptr);
	~RmGUIButton();
	virtual void layout(RmRectRaw client) override;
	virtual void paint(IRmGUIPainterRaw painter, RmRectRaw client) override;
	RmString getText() const;
	void setText(RmString const& text);
	RmGUIButtonStyle getStyle() const;
	void setStyle(RmGUIButtonStyle const& style);
	RmGUILabelStyle getTextStyle() const;
	void setTextStyle(RmGUILabelStyle const& style);
	RmFontAligns getAlignment() const;
	void setAlignment(RmFontAligns value);
	bool getChecked() const;
	void  setChecked(bool value);
	bool getCheckable() const;
	void  setCheckable(bool value);
	bool getDown() const;

protected:
	virtual void mouseDoubleEvent(IRmGUIMouseEventRaw event) override;
	virtual void mousePressEvent(IRmGUIMouseEventRaw event) override;
	virtual void mouseReleaseEvent(IRmGUIMouseEventRaw event) override;
	virtual void mouseMoveEvent(IRmGUIMouseEventRaw event) override;
	virtual void enterEvent(IRmGUIMouseEventRaw event) override;
	virtual void leaveEvent(IRmGUIMouseEventRaw event) override;

public:
	IRmGUISignalAsRaw<bool /*checked*/> clicked;
	IRmGUISignalAsRaw<> pressed;
	IRmGUISignalAsRaw<> released;
	IRmGUISignalAsRaw<> hovered;
	IRmGUISignalAsRaw<bool /*checked*/> toggled;

private:
	RmGUIWidgetPrivateRaw m_PrivateButton;
};
using RmGUIButtonRef = RmRef<RmGUIButton>;
using RmGUIButtonRaw = RmRaw<RmGUIButton>;