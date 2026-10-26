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
#include "UILabel.h"

/// @brief Button
class OPENUI_API UIButton : public UIWidget
{
public:
	explicit UIButton(UICanvasRaw canvas);
	~UIButton() override;
	void arrange(UIRect client) override;
	void paint(UIRect client, UIPainterRaw painter) override;
	bool setStyleText(UIString name, UIString value) override;

	UIString getText() const;
	void setText(UIString const& text);

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