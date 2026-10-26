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

/// @brief Progress
class OPENUI_API UIProgress : public UIWidget
{
public:
	explicit UIProgress(UICanvasRaw canvas);
	~UIProgress() override;
	void paint(UIRect client, UIPainterRaw painter) override;

	UIString getText() const;
	void setText(UIString const& text);

	int32_t getMinimum() const;
	int32_t getMaximum() const;
	void setRange(int32_t minimum, int32_t maximum);
	int32_t getValue() const;
	void setValue(int32_t value);

	UISignalAsRaw<int32_t> valueChanged;

private:
	UIPrivateRaw m_Private;
};
using UIProgressRef = UIRef<UIProgress>;
using UIProgressRaw = UIRaw<UIProgress>;
