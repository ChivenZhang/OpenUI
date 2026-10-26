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
#include "../UIWidget.h"
#include "../UIFactory.h"
#include "UIButton.h"

/// @brief Appearance copied onto the combo's buttons through setStyleText.
struct UIComboFace
{
	struct Mode
	{
		UIPen Pen;
		UIBrush Brush;
	};
	UIFloat2 Round{ 4, 4 };
	UILabelStyle Label =
	{
		.Normal
		{
			.Foreground {.Font { .Align = UIFont::AlignCenter | UIFont::AlignVCenter, }, .Brush = UICtrlInkBrush, },
			.Background{.Pen {.Style = UIPen::NoPen, }, .Brush {.Style = UIBrush::NoBrush, }},
		},
		.Hover
		{
			.Foreground {.Font { .Align = UIFont::AlignCenter | UIFont::AlignVCenter, }, .Brush = UICtrlInkBrush, },
			.Background{.Pen {.Style = UIPen::NoPen, }, .Brush {.Style = UIBrush::NoBrush, }},
		},
		.Disable
		{
			.Foreground {.Font { .Align = UIFont::AlignCenter | UIFont::AlignVCenter, }, .Brush = UICtrlDisabledInkBrush, },
			.Background{.Pen {.Style = UIPen::NoPen, }, .Brush {.Style = UIBrush::NoBrush, }},
		},
	};
	Mode Normal{ .Pen = UICtrlLinePen, .Brush = UICtrlPaperBrush };
	Mode Hover{ .Pen = UICtrlAccentPen, .Brush = UICtrlPaperBrush };
	Mode Press{ .Pen = UICtrlAccentPen, .Brush = UICtrlHoverBrush };
	Mode Disable{ .Pen = UICtrlDisabledPen, .Brush = UICtrlDisabledFillBrush };
};

/// @brief 
struct UIComboStyle
{
	UIComboFace Button
	{
		.Round{ 4, 4 },
		.Label
		{
			.Normal
			{
				.Foreground {.Font = {.Align = UIFont::AlignLeft | UIFont::AlignVCenter}, .Brush = UICtrlInkBrush, },
				.Background {.Pen = {.Style = UIPen::NoPen,}, .Brush = {.Style = UIBrush::NoBrush, },},
			},
			.Hover
			{
				.Foreground {.Font = {.Align = UIFont::AlignLeft | UIFont::AlignVCenter}, .Brush = UICtrlInkBrush, },
				.Background {.Pen = {.Style = UIPen::NoPen,}, .Brush = {.Style = UIBrush::NoBrush, },},
			},
			.Disable
			{
				.Foreground {.Font = {.Align = UIFont::AlignLeft | UIFont::AlignVCenter}, .Brush = UICtrlDisabledInkBrush, },
				.Background {.Pen = {.Style = UIPen::NoPen,}, .Brush = {.Style = UIBrush::NoBrush, },},
			},
		},
		.Normal
		{
			.Pen = UICtrlLinePen,
			.Brush = UICtrlPaperBrush,
		},
		.Hover
		{
			.Pen = UICtrlAccentPen,
			.Brush = UICtrlPaperBrush,
		},
		.Press
		{
			.Pen = UICtrlAccentPen,
			.Brush = UICtrlHoverBrush,
		},
		.Disable
		{
			.Pen = UICtrlDisabledPen,
			.Brush = UICtrlDisabledFillBrush,
		},
	};
	UIComboFace Items
	{
		.Round{ 0, 0 },
		.Label
		{
			.Normal
			{
				.Foreground {.Font = {.Align = UIFont::AlignLeft | UIFont::AlignVCenter}, .Brush = UICtrlInkBrush, },
				.Background {.Pen = {.Style = UIPen::NoPen,}, .Brush = {.Style = UIBrush::NoBrush,}},
			},
			.Hover
			{
				.Foreground{.Font = {.Align = UIFont::AlignLeft | UIFont::AlignVCenter}, .Brush = UICtrlInkBrush, },
				.Background{.Pen = {.Style = UIPen::NoPen,}, .Brush = {.Style = UIBrush::NoBrush,}},
			},
		},
		.Normal
		{
			.Pen = {.Style = UIPen::NoPen,},
			.Brush = { .Style = UIBrush::NoBrush, },
		},
		.Hover
		{
			.Pen = { .Style = UIPen::NoPen, },
			.Brush = UICtrlHoverBrush,
		},
		.Press
		{
			.Pen = { .Style = UIPen::NoPen, },
			.Brush = UICtrlPressBrush,
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
	bool setStyleText(UIString name, UIString value) override;

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