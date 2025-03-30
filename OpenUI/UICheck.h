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
#include "UILabel.h"

/// @brief 
struct UICheckStyle
{
	struct CheckMode
	{
		UIPen Pen;
		UIFont Font;
		UIBrush Brush;
	};
	UIFloat2 Round;
	CheckMode Normal, Hover, Press, Disable;
};

/// @brief Check
class OPENUI_API UICheck : public UIElement
{
public:
	explicit UICheck(UIContextRaw context);
	~UICheck() override;
	void arrange(UIRect client) override;
	void layout(UIRect client) override;
	void paint(UIRect client, UIPainterRaw painter) override;

	UIString getText() const;
	void setText(UIString const& text);

	UICheckStyle getStyle() const;
	void setStyle(UICheckStyle const& style);

	bool getChecked() const;
	void setChecked(bool value);

	UIButtonRaw getButton() const;
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

private:
	UIElementPrivateRaw m_PrivateCheck;
};
using UICheckRef = UIRef<UICheck>;
using UICheckRaw = UIRaw<UICheck>;

/// @brief Check Factory
class OPENUI_API UICheckFactory : public UIFactory
{
public:
	UIString getTagName() const override;
	UIElementRef getElement(UIString style) const override;
};