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
#include "../UIGrid.h"

/// @brief 
struct UIGridItem
{
	uint32_t Row = 0, Column = 0, RowSpan = 1, ColumnSpan = 1;
};

/// @brief 
class UIGridPrivate : public UIElementPrivate
{
public:
	UIGridStyle Style;
	UIMap<UIElementRaw, UIGridItem> GridItemMap;
	UIList<uint32_t> RowStretch, ColumnStretch;
	UIList<float> RowPercent, ColumnPercent, RowSummary, ColumnSummary;
};
#define PRIVATE() ((UIGridPrivate*) m_PrivateGrid)

UIGrid::UIGrid(UIContextRaw context)
	:
	UIElement(context)
{
	m_PrivateGrid = new UIGridPrivate;

	PRIVATE()->Style.Pen = { .Color = { 108 / 255.0f, 110 / 255.0f, 111 / 255.0f, 1.0f }, };
	PRIVATE()->Style.Brush = { .Color = { 238 / 255.0f, 238 / 255.0f, 242 / 255.0f, 1.0f }, };
}

UIGrid::~UIGrid()
{
	delete m_PrivateGrid; m_PrivateGrid = nullptr;
}

void UIGrid::arrange(UIRect client)
{
	for (size_t i = 0; i < getChildren().size(); ++i)
	{
		getChildren()[i]->setDisplayType(UI::DisplayNone);
		auto result = PRIVATE()->GridItemMap.find(getChildren()[i].get());
		if (result == PRIVATE()->GridItemMap.end())
		{
			getChildren()[i]->setDisplayType(UI::DisplayNone);
		}
		else
		{
			auto row = result->second.Row;
			auto col = result->second.Column;
			auto rowSpan = result->second.RowSpan;
			auto colSpan = result->second.ColumnSpan;
			auto posX = PRIVATE()->ColumnSummary[col];
			auto posY = PRIVATE()->RowSummary[row];
			auto width = 0.0f, height = 0.0f;
			for (size_t k = 0; k < rowSpan && row + k < getColumnCount(); ++k) height += PRIVATE()->RowPercent[row + k];
			for (size_t k = 0; k < colSpan && col + k < getColumnCount(); ++k) width += PRIVATE()->ColumnPercent[col + k];

			getChildren()[i]->setDisplayType(UI::DisplayFlex);
			getChildren()[i]->setPositionType(UI::PositionAbsolute);
			getChildren()[i]->setFixedPos(UIValueF(posX, UI::UnitPercent), UIValueF(posY, UI::UnitPercent));
			getChildren()[i]->setFixedSize(UIValueF(width, UI::UnitPercent), UIValueF(height, UI::UnitPercent));
		}
	}
}

void UIGrid::paint(UIRect client, UIPainterRaw painter)
{
	UIElement::paint(client, painter);
	painter->setPen(UINoPen);
	painter->setBrush(PRIVATE()->Style.Brush);
	painter->drawRect(client.X, client.Y, client.W, client.H);
}

void UIGrid::repaint(UIRect client, UIPainterRaw painter)
{
	UIElement::repaint(client, painter);
	painter->setPen(PRIVATE()->Style.Pen);
	painter->setBrush(UINoBrush);
	painter->drawRect(client.X, client.Y, client.W, client.H);
}

bool UIGrid::addElement(UIElementRef value)
{
	auto result = UIElement::addElement(value);
	if (result) PRIVATE()->GridItemMap.emplace(value.get(), UIGridItem{ 0,0,1,1 });
	return result;
}

bool UIGrid::addElement(UIElementRef value, uint32_t row, uint32_t column, uint32_t rowSpan, uint32_t columnSpan)
{
	auto result = UIElement::addElement(value);
	if (result) PRIVATE()->GridItemMap.emplace(value.get(), UIGridItem{ row, column, rowSpan, columnSpan });
	return result;
}

bool UIGrid::removeElement(UIElementRef value)
{
	auto result = UIElement::removeElement(value);
	if (result) PRIVATE()->GridItemMap.erase(value.get());
	return result;
}

void UIGrid::removeElement()
{
	UIElement::removeElement();
	PRIVATE()->GridItemMap.clear();
}

UIGridStyle UIGrid::getStyle() const
{
	return PRIVATE()->Style;
}

void UIGrid::setStyle(UIGridStyle value)
{
	PRIVATE()->Style = value;
}

size_t UIGrid::getRowCount() const
{
	return PRIVATE()->RowStretch.size();
}

uint32_t UIGrid::getRowStretch(size_t index) const
{
	if (index < PRIVATE()->RowStretch.size()) return PRIVATE()->RowStretch[index];
	return 0;
}

void UIGrid::setRowStretch(size_t index, uint32_t stretch)
{
	auto oldRow = getRowCount();
	auto oldCol = getColumnCount();
	auto newRow = std::max(oldRow, index + 1);
	auto newCol = std::max(oldCol, size_t(1));
	PRIVATE()->RowStretch.resize(newRow, 1);
	PRIVATE()->ColumnStretch.resize(newCol, 1);
	PRIVATE()->RowStretch[index] = std::max(1u, stretch);
	PRIVATE()->RowPercent.resize(PRIVATE()->RowStretch.size());
	PRIVATE()->ColumnPercent.resize(PRIVATE()->ColumnStretch.size());
	PRIVATE()->RowSummary.resize(PRIVATE()->RowStretch.size());
	PRIVATE()->ColumnSummary.resize(PRIVATE()->ColumnStretch.size());

	uint32_t rowStretches = 0, colStretches = 0;
	for (size_t i = 0; i < getRowCount(); ++i) rowStretches += getRowStretch(i);
	for (size_t i = 0; i < getColumnCount(); ++i) colStretches += getColumnStretch(i);
	uint32_t rowStretch = 0, colStretch = 0;
	for (size_t i = 0; i < getRowCount(); ++i)
	{
		PRIVATE()->RowPercent[i] = getRowStretch(i) * 100.0f / rowStretches;
		PRIVATE()->RowSummary[i] = rowStretch * 100.0f / rowStretches;
		rowStretch += getRowStretch(i);
	}
	for (size_t i = 0; i < getColumnCount(); ++i)
	{
		PRIVATE()->ColumnPercent[i] = getColumnStretch(i) * 100.0f / colStretches;
		PRIVATE()->ColumnSummary[i] = colStretch * 100.0f / colStretches;
		colStretch += getColumnStretch(i);
	}
}

void UIGrid::setRowStretch(UIList<uint32_t> stretch)
{
	PRIVATE()->RowStretch = stretch;
	PRIVATE()->RowPercent.resize(PRIVATE()->RowStretch.size());
	PRIVATE()->RowSummary.resize(PRIVATE()->RowStretch.size());

	for (size_t i = 0; i < PRIVATE()->RowStretch.size(); ++i)
	{
		PRIVATE()->RowStretch[i] = std::max(1u, PRIVATE()->RowStretch[i]);
	}

	uint32_t rowStretch = 0, rowStretches = 0;
	for (size_t i = 0; i < getRowCount(); ++i) rowStretches += getRowStretch(i);
	for (size_t i = 0; i < getRowCount(); ++i)
	{
		PRIVATE()->RowPercent[i] = getRowStretch(i) * 100.0f / rowStretches;
		PRIVATE()->RowSummary[i] = rowStretch * 100.0f / rowStretches;
		rowStretch += getRowStretch(i);
	}
}

size_t UIGrid::getColumnCount() const
{
	return PRIVATE()->ColumnStretch.size();
}

uint32_t UIGrid::getColumnStretch(size_t index) const
{
	if (index < PRIVATE()->ColumnStretch.size()) return PRIVATE()->ColumnStretch[index];
	return 0;
}

void UIGrid::setColumnStretch(size_t index, uint32_t stretch)
{
	auto oldRow = getRowCount();
	auto oldCol = getColumnCount();
	auto newRow = std::max(oldRow, size_t(1));
	auto newCol = std::max(oldCol, index + 1);
	PRIVATE()->RowStretch.resize(newRow, 1);
	PRIVATE()->ColumnStretch.resize(newCol, 1);
	PRIVATE()->ColumnStretch[index] = std::max(1u, stretch);
	PRIVATE()->RowPercent.resize(PRIVATE()->RowStretch.size());
	PRIVATE()->ColumnPercent.resize(PRIVATE()->ColumnStretch.size());
	PRIVATE()->RowSummary.resize(PRIVATE()->RowStretch.size());
	PRIVATE()->ColumnSummary.resize(PRIVATE()->ColumnStretch.size());

	uint32_t rowStretches = 0, colStretches = 0;
	for (size_t i = 0; i < getRowCount(); ++i) rowStretches += getRowStretch(i);
	for (size_t i = 0; i < getColumnCount(); ++i) colStretches += getColumnStretch(i);
	uint32_t rowStretch = 0, colStretch = 0;
	for (size_t i = 0; i < getRowCount(); ++i)
	{
		PRIVATE()->RowPercent[i] = getRowStretch(i) * 100.0f / rowStretches;
		PRIVATE()->RowSummary[i] = rowStretch * 100.0f / rowStretches;
		rowStretch += getRowStretch(i);
	}
	for (size_t i = 0; i < getColumnCount(); ++i)
	{
		PRIVATE()->ColumnPercent[i] = getColumnStretch(i) * 100.0f / colStretches;
		PRIVATE()->ColumnSummary[i] = colStretch * 100.0f / colStretches;
		colStretch += getColumnStretch(i);
	}
}

void UIGrid::setColumnStretch(UIList<uint32_t> stretch)
{
	PRIVATE()->ColumnStretch = stretch;
	PRIVATE()->ColumnPercent.resize(PRIVATE()->ColumnStretch.size());
	PRIVATE()->ColumnSummary.resize(PRIVATE()->ColumnStretch.size());

	for (size_t i = 0; i < PRIVATE()->ColumnStretch.size(); ++i)
	{
		PRIVATE()->ColumnStretch[i] = std::max(1u, PRIVATE()->ColumnStretch[i]);
	}

	uint32_t colStretch = 0, colStretches = 0;
	for (size_t i = 0; i < getColumnCount(); ++i) colStretches += getColumnStretch(i);
	for (size_t i = 0; i < getColumnCount(); ++i)
	{
		PRIVATE()->ColumnPercent[i] = getColumnStretch(i) * 100.0f / colStretches;
		PRIVATE()->ColumnSummary[i] = colStretch * 100.0f / colStretches;
		colStretch += getColumnStretch(i);
	}
}

UIString UIGridFactory::getTagName() const
{
	return "grid";
}

UIElementRef UIGridFactory::getElement(UIString style) const
{
	auto result = UINew<UIGrid>(getContext());
	result->setStyleText(style);
	return result;
}