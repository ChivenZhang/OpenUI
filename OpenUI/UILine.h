#pragma once
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
	UIHLine();
	~UIHLine();
	virtual void paint(UIRect client, UIPainterRaw painter) override;

	UILineStyle getStyle() const;
	void setStyle(UILineStyle value);

private:
	UIElementPrivateRaw m_PrivateLine;
};

/// @brief VLine
class OPENUI_API UIVLine : public UIElement
{
public:
	UIVLine();
	~UIVLine();
	virtual void paint(UIRect client, UIPainterRaw painter) override;

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