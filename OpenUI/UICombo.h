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
#include "UIButton.h"

/// @brief 
struct UIComboStyle
{
	UIButtonStyle Button
	{
		.Label
		{
			.Normal
			{
				.Foreground {.Font = {.Align = UIFont::AlignLeft | UIFont::AlignVCenter}, },
				.Background {.Pen = {.Style = UIPen::NoPen,}, .Brush = {.Style = UIBrush::NoBrush, },},
			},
			.Hover
			{
				.Foreground {.Font = {.Align = UIFont::AlignLeft | UIFont::AlignVCenter}, },
				.Background {.Pen = {.Style = UIPen::NoPen,}, .Brush = {.Style = UIBrush::NoBrush, },},
			},
		},
		.Normal
		{
			.Pen = UIButtonPenNormal,
			.Brush = UIButtonBrushNormal,
		},
		.Hover
		{
			.Pen = UIButtonPenHover,
			.Brush = UIButtonBrushHover,
		},
		.Press
		{
			.Pen = UIButtonPenPress,
			.Brush = UIButtonBrushPress,
		},
	};
	UIButtonStyle Items
	{
		.Round{ 0, 0 },
		.Label
		{
			.Normal
			{
				.Foreground {.Font = {.Align = UIFont::AlignLeft | UIFont::AlignVCenter}, },
				.Background {.Pen = {.Style = UIPen::NoPen,}, .Brush = {.Style = UIBrush::NoBrush,}},
			},
			.Hover
			{
				.Foreground{.Font = {.Align = UIFont::AlignLeft | UIFont::AlignVCenter}, },
				.Background{.Pen = {.Style = UIPen::NoPen,}, .Brush = {.Style = UIBrush::NoBrush,}},
			},
		},
		.Normal
		{
			.Pen = {.Style = UIPen::NoPen,},
			.Brush = UIButtonBrushNormal,
		},
		.Hover
		{
			.Pen = {.Style = UIPen::NoPen,},
			.Brush = UIButtonBrushHover,
		},
		.Press
		{
			.Pen = {.Style = UIPen::NoPen,},
			.Brush = UIButtonBrushPress,
		},
	};
};

/// @brief Combo
class OPENUI_API UICombo : public UIElement
{
public:
	explicit UICombo(UIContextRaw context);
	~UICombo() override;
	void arrange(UIRect client) override;
	void layout(UIRect client) override;
	void paint(UIRect client, UIPainterRaw painter) override;
	void repaint(UIRect client, UIPainterRaw painter) override;

	UIStringList const& getItems() const;
	void setItems(UIStringList const& texts);

	int32_t getMaxCount() const;
	void setMaxCount(int32_t value);

	int32_t getCurrentIndex() const;
	void setCurrentIndex(int32_t index);

	UIString getCurrentText() const;
	void setCurrentText(UIString const& text);

	UIComboStyle getStyle() const;
	void setStyle(UIComboStyle value);

protected:
	bool filter(UIReactorRaw source, UIEventRaw event) override;

public:
	UISignalAsRaw<int /*index*/> activated;
	UISignalAsRaw<int /*index*/> currentIndexChanged;
	UISignalAsRaw<UIString /*text*/> currentTextChanged;
	// UISignalAsRaw<UIString /*text*/> editTextChanged;
	UISignalAsRaw<int /*index*/> highlighted;
	UISignalAsRaw<UIString /*text*/> textActivated;
	UISignalAsRaw<UIString /*text*/> textHighlighted;

private:
	UIElementPrivateRaw m_PrivateCombo;
};
using UIComboRef = UIRef<UICombo>;
using UIComboRaw = UIRaw<UICombo>;

/// @brief Combo Factory
class OPENUI_API UIComboFactory : public UIFactory
{
public:
	UIString getTagName() const override;
	UIElementRef getElement(UIString style) const override;
};