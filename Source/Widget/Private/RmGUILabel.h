#pragma once
#include "RmGUIControl.h"

struct RmGUILabelTextStyle
{
	RmPen Pen;
	RmFont Font;
	RmBrush Brush;
};

class RMGUI_API RmGUILabel : public RmGUIControl
{
public:
	RmGUILabel(IRmGUIWidgetRaw parent = nullptr);
	~RmGUILabel();
	virtual void paint(IRmGUIPainterRaw painter, RmRectRaw client) override;
	RmString getText() const;
	void setText(RmString const& text);
	RmGUILabelTextStyle getStyle() const;
	void setStyle(RmGUILabelTextStyle const& style);
	RmFontAligns getAlignment() const;
	void setAlignment(RmFontAligns value);

protected:
	virtual void mouseDoubleEvent(IRmGUIMouseEventRaw event) override;
	virtual void mouseMoveEvent(IRmGUIMouseEventRaw event) override;
	virtual void mousePressEvent(IRmGUIMouseEventRaw event) override;
	virtual void mouseReleaseEvent(IRmGUIMouseEventRaw event) override;

public:
	IRmGUISignalAsRaw<RmString> linkHovered;
	IRmGUISignalAsRaw<RmString> linkActivated;

private:
	RmGUIWidgetPrivateRaw m_PrivateLabel;
};
using RmGUILabelRef = RmRef<RmGUILabel>;
using RmGUILabelRaw = RmRaw<RmGUILabel>;