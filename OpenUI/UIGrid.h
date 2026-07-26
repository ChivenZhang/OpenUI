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
struct UIGridStyle
{
	UIPen Pen;
	UIBrush Brush;
};

/// @brief Grid
class OPENUI_API UIGrid : public UIWidget
{
public:
	explicit UIGrid(UIContextRaw context);
	~UIGrid() override;
	void arrange(UIRect client) override;
	void paint(UIRect client, UIPainterRaw painter) override;
	void repaint(UIRect client, UIPainterRaw painter) override;

	bool addWidget(UIWidgetRef value) override;
	bool addElement(UIWidgetRef value, uint32_t row, uint32_t column, uint32_t rowSpan = 1, uint32_t columnSpan = 1);
	bool removeWidget(UIWidgetRef value) override;
	void removeWidget() override;

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
	UIWidgetPrivateRaw m_PrivateGrid;
};

/// @brief Grid factory
class OPENUI_API UIGridFactory : public UIFactory
{
public:
	UIString getTagName() const override;
	UIWidgetRef getElement(UIString style) const override;
};