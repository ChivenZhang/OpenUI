#pragma once
#include "RmGUIControl.h"

struct RmGUILabelStyle
{
	RmPen Pen;
	RmFont Font;
	RmBrush Brush;
};


class RMGUI_API RmGUILabel : public RmGUIControl
{
public:
	using scale_t = enum
	{
		ScaleFixed = 0,
		ScaleAuto = 1,
		ScaleNoRatio = 2,
		ScaleKeepRatio = 3,
	};

public:
	RmGUILabel(IRmGUIWidgetRaw parent = nullptr);
	~RmGUILabel();
	virtual void layout(RmRectRaw client) override;
	virtual void paint(IRmGUIPainterRaw painter, RmRectRaw client) override;

	RmString getText() const;
	void setText(RmString const& text);

	RmGUILabelStyle getStyle() const;
	void setStyle(RmGUILabelStyle const& style);

	RmFontAligns getAlignment() const;
	void setAlignment(RmFontAligns value);

	RmImage getPixmap() const;
	void setPixmap(RmImage image);

	scale_t getScaledContents() const;
	void setScaledContents(scale_t value);

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
using RmGUILabelScale = RmGUILabel::scale_t;