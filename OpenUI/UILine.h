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
struct UILineStyle
{
	UIPen Pen = UIFramePen;
	UIBrush Brush = UIFrameBrush;
};

/// @brief HLine
class OPENUI_API UIHLine : public UIWidget
{
public:
	explicit UIHLine(UICanvasRaw canvas);
	~UIHLine() override;
	void paint(UIRect client, UIPainterRaw painter) override;

	UILineStyle getStyle() const;
	void setStyle(UILineStyle value);

private:
	UIWidgetPrivateRaw m_PrivateLine;
};

/// @brief VLine
class OPENUI_API UIVLine : public UIWidget
{
public:
	explicit UIVLine(UICanvasRaw canvas);
	~UIVLine() override;
	void paint(UIRect client, UIPainterRaw painter) override;

	UILineStyle getStyle() const;
	void setStyle(UILineStyle value);

private:
	UIWidgetPrivateRaw m_PrivateLine;
};