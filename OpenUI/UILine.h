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
struct UILineStyle
{
	UIPen Pen = UIFramePen;
	UIBrush Brush = UIFrameBrush;
};

/// @brief HLine
class OPENUI_API UIHLine : public UIElement
{
public:
	explicit UIHLine(UIContextRaw context);
	~UIHLine() override;
	void paint(UIRect client, UIPainterRaw painter) override;

	UILineStyle getStyle() const;
	void setStyle(UILineStyle value);

private:
	UIElementPrivateRaw m_PrivateLine;
};

/// @brief VLine
class OPENUI_API UIVLine : public UIElement
{
public:
	explicit UIVLine(UIContextRaw context);
	~UIVLine() override;
	void paint(UIRect client, UIPainterRaw painter) override;

	UILineStyle getStyle() const;
	void setStyle(UILineStyle value);

private:
	UIElementPrivateRaw m_PrivateLine;
};

/// @brief HLine factory
class UIHLineFactory : public UIFactory
{
public:
	UIString getTagName() const override;
	UIElementRef getElement(UIString style) const override;
};

/// @brief VLine factory
class UIVLineFactory : public UIFactory
{
public:
	UIString getTagName() const override;
	UIElementRef getElement(UIString style) const override;
};