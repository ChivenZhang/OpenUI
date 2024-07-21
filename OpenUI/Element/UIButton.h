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
	UILabelStyle Label;
	UIFloat2 Round = { 5, 5 };
	ButtonMode Normal = { .Pen = {.Color = {0 / 255.0f, 120 / 255.0f, 212 / 255.0f, 1.0f} }, .Brush = {.Color = {253 / 255.0f, 253 / 255.0f, 253 / 255.0f, 1.0f} } };
	ButtonMode Hover = { .Pen = {.Color = {0 / 255.0f, 120 / 255.0f, 212 / 255.0f, 1.0f} }, .Brush = {.Color = {224 / 255.0f, 238 / 255.0f, 249 / 255.0f, 1.0f} } };
	ButtonMode Press = { .Pen = {.Color = {0 / 255.0f, 120 / 255.0f, 212 / 255.0f, 1.0f} }, .Brush = {.Color = {204 / 255.0f, 228 / 255.0f, 247 / 255.0f, 1.0f} } };
	ButtonMode Disable = { .Pen = {.Color = {0 / 255.0f, 120 / 255.0f, 212 / 255.0f, 1.0f} }, };
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

/// @brief Button factory
class OPENUI_API UIButtonFactory : public UIFactory
{
public:
	UIString getTagName() const override;
	UIElementRef getElement(UIString style) const override;
};