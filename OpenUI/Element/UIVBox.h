#pragma once
#include "OpenUI/UIElement.h"
#include "OpenUI/UIFactory.h"

/// @brief 
struct UIVBoxStyle
{
	UIPen Pen;
	UIBrush Brush;
};

/// @brief VBox
class OPENUI_API UIVBox : public UIElement
{
public:
	UIVBox();
	~UIVBox();
	virtual void arrange(UIRect client) override;
	virtual void paint(UIRect client, UIPainterRaw painter) override;

	UIVBoxStyle getStyle() const;
	void setStyle(UIVBoxStyle value);

private:
	UIElementPrivateRaw m_PrivateVBox;
};

/// @brief VBox factory
class OPENUI_API UIVBoxFactory : public UIFactory
{
public:
	virtual UIString getTagName() const override;
	virtual UIElementRef getElement(UIString style) const override;
};