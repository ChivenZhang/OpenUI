#pragma once
#include "UIElement.h"
#include "UIFactory.h"

/// @brief 
struct UIFlowStyle
{
	UIPen Pen;
	UIBrush Brush;
};

/// @brief Flow
class OPENUI_API UIFlow : public UIElement
{
public:
	UIFlow();
	~UIFlow();
	virtual void arrange(UIRect client) override;
	virtual void paint(UIRect client, UIPainterRaw painter) override;

	UIFlowStyle getStyle() const;
	void setStyle(UIFlowStyle value);

private:
	UIElementPrivateRaw m_PrivateFlow;
};

/// @brief Flow factory
class OPENUI_API UIFlowFactory : public UIFactory
{
public:
	virtual UIString getTagName() const override;
	virtual UIElementRef getElement(UIString style) const override;
};