#pragma once
#include "OpenUI/UIElement.h"
#include "OpenUI/UIFactory.h"

/// @brief 
struct UIHBoxStyle
{
	UIPen Pen;
	UIBrush Brush;
};

/// @brief HBox
class OPENUI_API UIHBox : public UIElement
{
public:
	UIHBox();
	~UIHBox();
	virtual void arrange(UIRect client) override;
	virtual void paint(UIRect client, UIPainterRaw painter) override;

	UIHBoxStyle getStyle() const;
	void setStyle(UIHBoxStyle value);

private:
	UIElementPrivateRaw m_PrivateHbox;
};

/// @brief HBox factory
class OPENUI_API UIHBoxFactory : public UIFactory
{
public:
	virtual UIString getTagName() const override;
	virtual UIElementRef getElement(UIString style) const override;
};