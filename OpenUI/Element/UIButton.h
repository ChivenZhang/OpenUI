#pragma once
#include "OpenUI/UIElement.h"
#include "OpenUI/UIFactory.h"
#include "UILabel.h"

/// @brief 
struct UIButtonStyle
{
	struct ButtonMode
	{
		UIPen Pen;
		UIBrush Brush;
	};
	UIFloat2 Round;
	ButtonMode Normal, Hover, Press, Disable;
};

/// @brief Button
class OPENUI_API UIButton : public UIElement
{
public:
	UIButton();
	~UIButton();
	virtual void arrange(UIRect client) override;
	virtual void paint(UIRect client, UIPainterRaw painter) override;

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
	UISignalAsRaw<bool /*checked*/> toggled;

private:
	UIElementPrivateRaw m_PrivateButton;
};
using UIButtonRef = UIRef<UIButton>;
using UIButtonRaw = UIRaw<UIButton>;

/// @brief Button Factory
class OPENUI_API UIButtonFactory : public UIFactory
{
public:
	UIString getTagName() const override;
	UIElementRef getElement(UIString style) const override;
};