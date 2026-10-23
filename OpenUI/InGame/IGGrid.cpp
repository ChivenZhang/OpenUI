#include "IGGrid.h"

struct IGGridCell
{
	uint32_t Row = 0, Column = 0, RowSpan = 1, ColumnSpan = 1;
};

struct IGGridPrivate : UIPrivate
{
	IGFrameStyle Style;
	UIMap<UIWidgetRaw, IGGridCell> Cells;
	UIList<uint32_t> RowStretch, ColumnStretch;
	UIList<float> RowPercent, ColumnPercent, RowSummary, ColumnSummary;

	void reflow()
	{
		RowPercent.resize(RowStretch.size());
		ColumnPercent.resize(ColumnStretch.size());
		RowSummary.resize(RowStretch.size());
		ColumnSummary.resize(ColumnStretch.size());
		uint32_t rowSum = 0, columnSum = 0;
		for (auto value : RowStretch) rowSum += value;
		for (auto value : ColumnStretch) columnSum += value;
		if (rowSum == 0) rowSum = 1;
		if (columnSum == 0) columnSum = 1;
		uint32_t rowUsed = 0, columnUsed = 0;
		for (size_t i = 0; i < RowStretch.size(); ++i)
		{
			RowPercent[i] = RowStretch[i] * 100.0f / rowSum;
			RowSummary[i] = rowUsed * 100.0f / rowSum;
			rowUsed += RowStretch[i];
		}
		for (size_t i = 0; i < ColumnStretch.size(); ++i)
		{
			ColumnPercent[i] = ColumnStretch[i] * 100.0f / columnSum;
			ColumnSummary[i] = columnUsed * 100.0f / columnSum;
			columnUsed += ColumnStretch[i];
		}
	}

	void ensure(uint32_t rows, uint32_t columns)
	{
		if (RowStretch.size() < rows) RowStretch.resize(rows, 1);
		if (ColumnStretch.size() < columns) ColumnStretch.resize(columns, 1);
		reflow();
	}
};
#define PRIVATE() ((IGGridPrivate*)m_Private)

IGGrid::IGGrid(UICanvasRaw canvas)
	:
	UIWidget(canvas)
{
	m_Private = new IGGridPrivate;
}

IGGrid::~IGGrid()
{
	delete m_Private; m_Private = nullptr;
}

void IGGrid::arrange(UIRect client)
{
	(void)client;
	for (size_t i = 0; i < getWidgets().size(); ++i)
	{
		auto child = getWidgets()[i];
		child->setDisplayType({UI_CSS_DISPLAY_NONE});
		auto found = PRIVATE()->Cells.find(child.get());
		if (found == PRIVATE()->Cells.end()) continue;
		auto cell = found->second;
		float width = 0, height = 0;
		for (uint32_t k = 0; k < cell.RowSpan && cell.Row + k < PRIVATE()->RowPercent.size(); ++k)
			height += PRIVATE()->RowPercent[cell.Row + k];
		for (uint32_t k = 0; k < cell.ColumnSpan && cell.Column + k < PRIVATE()->ColumnPercent.size(); ++k)
			width += PRIVATE()->ColumnPercent[cell.Column + k];
		float x = cell.Column < PRIVATE()->ColumnSummary.size() ? PRIVATE()->ColumnSummary[cell.Column] : 0;
		float y = cell.Row < PRIVATE()->RowSummary.size() ? PRIVATE()->RowSummary[cell.Row] : 0;
		child->setDisplayType({UI_CSS_DISPLAY_FLEX});
		child->setPositionType({UI_CSS_POSITION_ABSOLUTE});
		child->setFixedPos({x, UI_CSS_LEFT_PERCENTAGE}, {y, UI_CSS_TOP_PERCENTAGE});
		child->setFixedSize({width, UI_CSS_WIDTH_PERCENTAGE}, {height, UI_CSS_HEIGHT_PERCENTAGE});
	}
}

void IGGrid::paint(UIRect client, UIPainterRaw painter)
{
	UIWidget::paint(client, painter);
	IGDrawBox(painter, client, PRIVATE()->Style.Pen, PRIVATE()->Style.Brush, 0);
}

bool IGGrid::addWidget(UIWidgetRef value)
{
	return addWidget(value, 0, 0, 1, 1);
}

bool IGGrid::addWidget(UIWidgetRef value, uint32_t row, uint32_t column, uint32_t rowSpan, uint32_t columnSpan)
{
	if (UIWidget::addWidget(value) == false) return false;
	PRIVATE()->Cells[value.get()] = { row, column, std::max(1u, rowSpan), std::max(1u, columnSpan) };
	PRIVATE()->ensure(row + std::max(1u, rowSpan), column + std::max(1u, columnSpan));
	return true;
}

bool IGGrid::removeWidget(UIWidgetRef value)
{
	if (UIWidget::removeWidget(value) == false) return false;
	PRIVATE()->Cells.erase(value.get());
	return true;
}

void IGGrid::removeWidget()
{
	UIWidget::removeWidget();
	PRIVATE()->Cells.clear();
}

void IGGrid::setRowStretch(UIList<uint32_t> stretch)
{
	PRIVATE()->RowStretch = stretch;
	for (auto& value : PRIVATE()->RowStretch) value = std::max(1u, value);
	PRIVATE()->reflow();
}

void IGGrid::setColumnStretch(UIList<uint32_t> stretch)
{
	PRIVATE()->ColumnStretch = stretch;
	for (auto& value : PRIVATE()->ColumnStretch) value = std::max(1u, value);
	PRIVATE()->reflow();
}

IGFrameStyle IGGrid::getStyle() const
{
	return PRIVATE()->Style;
}

void IGGrid::setStyle(IGFrameStyle value)
{
	PRIVATE()->Style = value;
}
#undef PRIVATE
