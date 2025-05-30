#pragma once
/*=================================================
* Copyright © 2020-2025 ChivenZhang.
* All Rights Reserved.
* =====================Note=========================
*
*
* ====================History=======================
* Created by ChivenZhang@gmail.com.
*
* =================================================*/
#include "UIElement.h"
#include "UIFactory.h"

/// @brief 
struct UILabelStyle
{
	struct LabelMode
	{
		struct
		{
			UIPen Pen{ .Style = UIPen::NoPen, };
			UIFont Font{ .Align = UIFont::AlignLeft | UIFont::AlignVCenter, };
			UIBrush Brush{ .Color {0,0,0,1}, };
		} Foreground;
		struct
		{
			UIPen Pen = UIFramePen;
			UIBrush Brush = UIFrameBrush;
		} Background;
	};
	LabelMode Normal, Hover, Disable;
};

/// @brief Label
class OPENUI_API UILabel : public UIElement
{
public:
	using scale_t = enum
	{
		ScaleFixed = 0,
		ScaleAuto,
		ScaleNoRatio,
		ScaleKeepRatio,
	};

public:
	explicit UILabel(UIContextRaw context);
	~UILabel() override;
	void layout(UIRect client) override;
	void paint(UIRect client, UIPainterRaw painter) override;
	void repaint(UIRect client, UIPainterRaw painter) override;

	UIString getText() const;
	void setText(UIString const& text);

	UILabelStyle getStyle() const;
	void setStyle(UILabelStyle const& style);

	UIFontAligns getAlignment() const;
	void setAlignment(UIFontAligns value);

	UIImage getPixmap() const;
	void setPixmap(UIImage image);

	scale_t getScaledContents() const;
	void setScaledContents(scale_t value);

protected:
	void mouseDoubleEvent(UIMouseEventRaw event) override;
	void mousePressEvent(UIMouseEventRaw event) override;
	void mouseReleaseEvent(UIMouseEventRaw event) override;
	void mouseMoveEvent(UIMouseEventRaw event) override;
	void enterEvent(UIMouseEventRaw event) override;
	void leaveEvent(UIMouseEventRaw event) override;

public:
	UISignalAsRaw<UIString> linkHovered;
	UISignalAsRaw<UIString> linkActivated;

private:
	UIElementPrivateRaw m_PrivateLabel;
};
using UILabelRef = UIRef<UILabel>;
using UILabelRaw = UIRaw<UILabel>;

/// @brief Label factory
class OPENUI_API UILabelFactory : public UIFactory
{
public:
	UIString getTagName() const override;
	UIElementRef getElement(UIString style) const override;
};