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
struct UIVBoxStyle
{
	UIPen Pen = UIFramePen;
	UIBrush Brush = UIFrameBrush;
};

/// @brief VBox
class OPENUI_API UIVBox : public UIWidget
{
public:
	explicit UIVBox(UIContextRaw context);
	~UIVBox() override;
	void arrange(UIRect client) override;
	void paint(UIRect client, UIPainterRaw painter) override;
	void repaint(UIRect client, UIPainterRaw painter) override;

	UIVBoxStyle getStyle() const;
	void setStyle(UIVBoxStyle value);

private:
	UIWidgetPrivateRaw m_PrivateVBox;
};

/// @brief VBox factory
class OPENUI_API UIVBoxFactory : public UIFactory
{
public:
	UIString getTagName() const override;
	UIWidgetRef newWidget(UIString style) const override;
};