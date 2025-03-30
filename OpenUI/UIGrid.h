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
struct UIGridStyle
{
	UIPen Pen;
	UIBrush Brush;
};

/// @brief Grid
class OPENUI_API UIGrid : public UIElement
{
public:
	explicit UIGrid(UIContextRaw context);
	~UIGrid() override;
	void arrange(UIRect client) override;
	void paint(UIRect client, UIPainterRaw painter) override;
	void repaint(UIRect client, UIPainterRaw painter) override;

	bool addElement(UIElementRef value) override;
	bool addElement(UIElementRef value, uint32_t row, uint32_t column, uint32_t rowSpan = 1, uint32_t columnSpan = 1);
	bool removeElement(UIElementRef value) override;
	void removeElement() override;

	UIGridStyle getStyle() const;
	void setStyle(UIGridStyle value);

	size_t getRowCount() const;
	uint32_t getRowStretch(size_t index) const;
	void setRowStretch(size_t index, uint32_t stretch);
	void setRowStretch(UIList<uint32_t> stretch);

	size_t getColumnCount() const;
	uint32_t getColumnStretch(size_t index) const;
	void setColumnStretch(size_t index, uint32_t stretch);
	void setColumnStretch(UIList<uint32_t> stretch);

private:
	UIElementPrivateRaw m_PrivateGrid;
};

/// @brief Grid factory
class OPENUI_API UIGridFactory : public UIFactory
{
public:
	UIString getTagName() const override;
	UIElementRef getElement(UIString style) const override;
};