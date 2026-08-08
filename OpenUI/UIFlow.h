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
#include "UIWidget.h"
#include "UIFactory.h"

/// @brief 
struct UIFlowStyle
{
	UIPen Pen;
	UIBrush Brush;
};

/// @brief Flow
class OPENUI_API UIFlow : public UIWidget
{
public:
	explicit UIFlow(UICanvasRaw canvas);
	~UIFlow() override;
	void arrange(UIRect client) override;
	void paint(UIRect client, UIPainterRaw painter) override;

	UIFlowStyle getStyle() const;
	void setStyle(UIFlowStyle value);

private:
	UIWidgetPrivateRaw m_PrivateFlow;
};