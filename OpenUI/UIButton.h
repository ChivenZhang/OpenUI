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
#include "UIWidget.h"
#include "UIFactory.h"
#include "UILabel.h"

static constexpr UIPen UIButtonPenNormal = UIAccentPen;
static constexpr UIPen UIButtonPenHover = UIAccentHoverPen;
static constexpr UIPen UIButtonPenPress = UIAccentPressPen;
static constexpr UIPen UIButtonPenDisable = UIDisabledPen;
static constexpr UIBrush UIButtonBrushNormal = UIFrameBrush;
static constexpr UIBrush UIButtonBrushHover = UIAccentHoverBrush;
static constexpr UIBrush UIButtonBrushPress = UIAccentPressBrush;
static constexpr UIBrush UIButtonBrushDisable = UIDisabledBrush;

/// @brief 
struct UIButtonStyle
{
	struct ButtonMode
	{
		UIPen Pen;
		UIBrush Brush;
	};
	UIFloat2 Round{ 2, 2, };
	UILabelStyle Label =
	{
		.Normal
		{
			.Foreground {.Font { .Align = UIFont::AlignCenter | UIFont::AlignVCenter, }, .Brush = UITextBrush, },
			.Background{.Pen {.Style = UIPen::NoPen, }, .Brush {.Style = UIBrush::NoBrush, }},
		},
		.Hover
		{
			.Foreground {.Font { .Align = UIFont::AlignCenter | UIFont::AlignVCenter, }, .Brush = UIAccentBrush, },
			.Background{.Pen {.Style = UIPen::NoPen, }, .Brush {.Style = UIBrush::NoBrush, }},
		},
		.Disable
		{
			.Foreground {.Font { .Align = UIFont::AlignCenter | UIFont::AlignVCenter, }, .Brush = UIMutedBrush, },
			.Background{.Pen {.Style = UIPen::NoPen, }, .Brush {.Style = UIBrush::NoBrush, }},
		},
	};
	ButtonMode Normal{ .Pen = UIButtonPenNormal, .Brush = UIButtonBrushNormal };
	ButtonMode Hover{ .Pen = UIButtonPenHover, .Brush = UIButtonBrushHover };
	ButtonMode Press{ .Pen = UIButtonPenPress, .Brush = UIButtonBrushPress };
	ButtonMode Disable{ .Pen = UIButtonPenDisable, .Brush = UIButtonBrushDisable, };
};

/// @brief Button
class OPENUI_API UIButton : public UIWidget
{
public:
	explicit UIButton(UICanvasRaw canvas);
	~UIButton() override;
	void arrange(UIRect client) override;
	void paint(UIRect client, UIPainterRaw painter) override;

	UIString getText() const;
	void setText(UIString const& text);

	UIButtonStyle getStyle() const;
	void setStyle(UIButtonStyle const& style);

	bool getChecked() const;
	void setChecked(bool value);

	bool getCheckable() const;
	void setCheckable(bool value);

	bool getDown() const;

	UILabelRaw getLabel() const;

protected:
	void mouseDoubleEvent(UIMouseEventRaw event) override;
	void mousePressEvent(UIMouseEventRaw event) override;
	void mouseReleaseEvent(UIMouseEventRaw event) override;
	void mouseMoveEvent(UIMouseEventRaw event) override;
	void enterEvent(UIMouseEventRaw event) override;
	void leaveEvent(UIMouseEventRaw event) override;

public:
	UISignalAsRaw<bool /*checked*/> clicked;
	UISignalAsRaw<> pressed;
	UISignalAsRaw<> released;
	UISignalAsRaw<> hovered;
	UISignalAsRaw<bool /*checked*/> toggled;

private:
	UIPrivateRaw m_Private;
};
using UIButtonRef = UIRef<UIButton>;
using UIButtonRaw = UIRaw<UIButton>;