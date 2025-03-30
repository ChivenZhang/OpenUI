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
#include "UILabel.h"

static constexpr UIPen UIButtonPenNormal
{
	.Color {0 / 255.0f, 120 / 255.0f, 212 / 255.0f, 1.0f},
};
static constexpr UIPen UIButtonPenHover
{
	.Color {0 / 255.0f, 120 / 255.0f, 212 / 255.0f, 1.0f},
};
static constexpr UIPen UIButtonPenPress
{
	.Color {0 / 255.0f, 120 / 255.0f, 212 / 255.0f, 1.0f},
};
static constexpr UIPen UIButtonPenDisable
{
	.Color {0 / 255.0f, 120 / 255.0f, 212 / 255.0f, 1.0f},
};
static constexpr UIBrush UIButtonBrushNormal
{
	.Color {253 / 255.0f, 253 / 255.0f, 253 / 255.0f, 1.0f},
};
static constexpr UIBrush UIButtonBrushHover
{
	.Color {224 / 255.0f, 238 / 255.0f, 249 / 255.0f, 1.0f},
};
static constexpr UIBrush UIButtonBrushPress
{
	.Color {204 / 255.0f, 228 / 255.0f, 247 / 255.0f, 1.0f},
};
static constexpr UIBrush UIButtonBrushDisable
{
	.Color {0.8f, 0.8f, 0.8f, 1.0f},
};

/// @brief 
struct UIButtonStyle
{
	struct ButtonMode
	{
		UIPen Pen;
		UIBrush Brush;
	};
	UIFloat2 Round{ 0, 0 };
	UILabelStyle Label =
	{
		.Normal {.Foreground {.Font {.Align = UIFont::AlignCenter | UIFont::AlignVCenter, }}, .Background{.Pen {.Style = UIPen::NoPen, }, .Brush {.Style = UIBrush::NoBrush, }},},
		.Hover {.Foreground {.Font {.Align = UIFont::AlignCenter | UIFont::AlignVCenter, }}, .Background{.Pen {.Style = UIPen::NoPen, }, .Brush {.Style = UIBrush::NoBrush, }}},
	};
	ButtonMode Normal{ .Pen = UIButtonPenNormal, .Brush = UIButtonBrushNormal };
	ButtonMode Hover{ .Pen = UIButtonPenHover, .Brush = UIButtonBrushHover };
	ButtonMode Press{ .Pen = UIButtonPenPress, .Brush = UIButtonBrushPress };
	ButtonMode Disable{ .Pen = UIButtonPenDisable, };
};

/// @brief Button
class OPENUI_API UIButton : public UIElement
{
public:
	explicit UIButton(UIContextRaw context);
	~UIButton() override;
	void arrange(UIRect client) override;
	void paint(UIRect client, UIPainterRaw painter) override;
	void repaint(UIRect client, UIPainterRaw painter) override;

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
	UIElementPrivateRaw m_PrivateButton;
};
using UIButtonRef = UIRef<UIButton>;
using UIButtonRaw = UIRaw<UIButton>;

/// @brief Button factory
class OPENUI_API UIButtonFactory : public UIFactory
{
public:
	UIString getTagName() const override;
	UIElementRef getElement(UIString style) const override;
};