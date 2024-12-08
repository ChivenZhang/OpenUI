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
struct UIRadioStyle
{
	struct RadioMode
	{
		UIPen Pen;
		UIFont Font;
		UIBrush Brush;
	};
	UIFloat2 Round;
	RadioMode Normal, Hover, Press, Disable;
};

class UIRadioGroup;
using UIRadioGroupRef = UIRef<UIRadioGroup>;
using UIRadioGroupRaw = UIRaw<UIRadioGroup>;

/// @brief Radio
class OPENUI_API UIRadio : public UIElement
{
public:
	UIRadio();
	~UIRadio();
	virtual void arrange(UIRect client) override;
	virtual void layout(UIRect client) override;
	virtual void paint(UIRect client, UIPainterRaw painter) override;

	UIString getText() const;
	void setText(UIString const& text);

	UIRadioStyle getStyle() const;
	void setStyle(UIRadioStyle const& style);

	bool getChecked() const;
	void setChecked(bool value);

	UIRadioGroupRef getExclusive() const;
	void setExclusive(UIRadioGroupRef group);

	UIButtonRaw getButton() const;
	UILabelRaw getLabel() const;

protected:
	virtual void mouseDoubleEvent(UIMouseEventRaw event) override;
	virtual void mousePressEvent(UIMouseEventRaw event) override;
	virtual void mouseReleaseEvent(UIMouseEventRaw event) override;
	virtual void mouseMoveEvent(UIMouseEventRaw event) override;
	virtual void enterEvent(UIMouseEventRaw event) override;
	virtual void leaveEvent(UIMouseEventRaw event) override;

public:
	UISignalAsRaw<bool /*checked*/> clicked;
	UISignalAsRaw<> pressed;
	UISignalAsRaw<> released;
	UISignalAsRaw<> hovered;

private:
	UIElementPrivateRaw m_PrivateRadio;
};
using UIRadioRef = UIRef<UIRadio>;
using UIRadioRaw = UIRaw<UIRadio>;

/// @brief 
class UIRadioGroup
{
private:
	friend class UIRadio;
	UIRadioRaw Active = nullptr;
};

/// @brief Radio Factory
class OPENUI_API UIRadioFactory : public UIFactory
{
public:
	UIString getTagName() const override;
	UIElementRef getElement(UIString style) const override;
};