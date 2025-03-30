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
struct UIHBoxStyle
{
	UIPen Pen = UIFramePen;
	UIBrush Brush = UIFrameBrush;
};

/// @brief HBox
class OPENUI_API UIHBox : public UIElement
{
public:
	explicit UIHBox(UIContextRaw context);
	~UIHBox() override;
	void arrange(UIRect client) override;
	void paint(UIRect client, UIPainterRaw painter) override;
	void repaint(UIRect client, UIPainterRaw painter) override;

	UIHBoxStyle getStyle() const;
	void setStyle(UIHBoxStyle value);

private:
	UIElementPrivateRaw m_PrivateHbox;
};

/// @brief HBox factory
class OPENUI_API UIHBoxFactory : public UIFactory
{
public:
	UIString getTagName() const override;
	UIElementRef getElement(UIString style) const override;
};