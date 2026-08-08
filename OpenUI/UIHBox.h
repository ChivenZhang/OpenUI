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
struct UIHBoxStyle
{
	UIPen Pen = UIFramePen;
	UIBrush Brush = UIFrameBrush;
};

/// @brief HBox
class OPENUI_API UIHBox : public UIWidget
{
public:
	explicit UIHBox(UICanvasRaw canvas);
	~UIHBox() override;
	void arrange(UIRect client) override;
	void paint(UIRect client, UIPainterRaw painter) override;
	void repaint(UIRect client, UIPainterRaw painter) override;

	UIHBoxStyle getStyle() const;
	void setStyle(UIHBoxStyle value);

private:
	UIWidgetPrivateRaw m_PrivateHbox;
};