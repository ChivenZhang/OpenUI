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
#include "UIButton.h"

/// @brief Appearance copied onto the combo's buttons through setStyleText.
struct UIComboFace
{
	struct Mode
	{
		UIPen Pen;
		UIBrush Brush;
	};
	UIFloat2 Round{ 2, 2 };
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
	Mode Normal{ .Pen = UIAccentPen, .Brush = UIFrameBrush };
	Mode Hover{ .Pen = UIAccentHoverPen, .Brush = UIAccentHoverBrush };
	Mode Press{ .Pen = UIAccentPressPen, .Brush = UIAccentPressBrush };
	Mode Disable{ .Pen = UIDisabledPen, .Brush = UIDisabledBrush };
};

/// @brief 
struct UIComboStyle
{
	UIComboFace Button
	{
		.Round{ 2, 2 },
		.Label
		{
			.Normal
			{
				.Foreground {.Font = {.Align = UIFont::AlignLeft | UIFont::AlignVCenter}, .Brush = UITextBrush, },
				.Background {.Pen = {.Style = UIPen::NoPen,}, .Brush = {.Style = UIBrush::NoBrush, },},
			},
			.Hover
			{
				.Foreground {.Font = {.Align = UIFont::AlignLeft | UIFont::AlignVCenter}, .Brush = UITextBrush, },
				.Background {.Pen = {.Style = UIPen::NoPen,}, .Brush = {.Style = UIBrush::NoBrush, },},
			},
			.Disable
			{
				.Foreground {.Font = {.Align = UIFont::AlignLeft | UIFont::AlignVCenter}, .Brush = UIMutedBrush, },
				.Background {.Pen = {.Style = UIPen::NoPen,}, .Brush = {.Style = UIBrush::NoBrush, },},
			},
		},
		.Normal
		{
			.Pen = UIFramePen,
			.Brush = UIInsetBrush,
		},
		.Hover
		{
			.Pen = UIAccentHoverPen,
			.Brush = UIInsetBrush,
		},
		.Press
		{
			.Pen = UIAccentPen,
			.Brush = UIAccentSoftBrush,
		},
		.Disable
		{
			.Pen = UIDisabledPen,
			.Brush = UIDisabledBrush,
		},
	};
	UIComboFace Items
	{
		.Round{ 0, 0 },
		.Label
		{
			.Normal
			{
				.Foreground {.Font = {.Align = UIFont::AlignLeft | UIFont::AlignVCenter}, .Brush = UITextBrush, },
				.Background {.Pen = {.Style = UIPen::NoPen,}, .Brush = {.Style = UIBrush::NoBrush,}},
			},
			.Hover
			{
				.Foreground{.Font = {.Align = UIFont::AlignLeft | UIFont::AlignVCenter}, .Brush = UIAccentBrush, },
				.Background{.Pen = {.Style = UIPen::NoPen,}, .Brush = {.Style = UIBrush::NoBrush,}},
			},
		},
		.Normal
		{
			.Pen = {.Style = UIPen::NoPen,},
			.Brush = UIInsetBrush,
		},
		.Hover
		{
			.Pen = UIAccentPen,
			.Brush = UIAccentSoftBrush,
		},
		.Press
		{
			.Pen = UIAccentPen,
			.Brush = UIAccentHoverBrush,
		},
	};
};

/// @brief Combo
class OPENUI_API UICombo : public UIWidget
{
public:
	explicit UICombo(UICanvasRaw canvas);
	~UICombo() override;
	void arrange(UIRect client) override;
	void layout(UIRect client) override;
	void paint(UIRect client, UIPainterRaw painter) override;

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
	UIPrivateRaw m_Private;
};
using UIComboRef = UIRef<UICombo>;
using UIComboRaw = UIRaw<UICombo>;