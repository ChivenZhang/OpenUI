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
	explicit UIFlow(UIContextRaw context);
	~UIFlow() override;
	void arrange(UIRect client) override;
	void paint(UIRect client, UIPainterRaw painter) override;

	UIFlowStyle getStyle() const;
	void setStyle(UIFlowStyle value);

private:
	UIElementPrivateRaw m_PrivateFlow;
};

/// @brief Flow factory
class OPENUI_API UIFlowFactory : public UIFactory
{
public:
	UIString getTagName() const override;
	UIElementRef getElement(UIString style) const override;
};