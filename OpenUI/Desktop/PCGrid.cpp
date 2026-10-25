#include "PCGrid.h"
#include <algorithm>

struct PCGridPrivate : UIPrivate
{
	int32_t Columns = 2;
};
#define PRIVATE() ((PCGridPrivate*)m_Private)

PCGrid::PCGrid(UICanvasRaw canvas)
	:
	UIWidget(canvas)
{
	m_Private = new PCGridPrivate;
}

PCGrid::~PCGrid()
{
	delete m_Private; m_Private = nullptr;
}

void PCGrid::arrange(UIRect client)
{
	(void)client;
	int32_t columns = std::max(1, PRIVATE()->Columns);
	int32_t count = (int32_t)getWidgets().size();
	int32_t rows = std::max(1, (count + columns - 1) / columns);
	float cellW = 100.0f / (float)columns;
	float cellH = 100.0f / (float)rows;
	for (int32_t i = 0; i < count; ++i)
	{
		auto child = getWidgets()[i];
		int32_t column = i % columns;
		int32_t row = i / columns;
		child->setDisplayType({ UI_CSS_DISPLAY_FLEX });
		child->setPositionType({ UI_CSS_POSITION_ABSOLUTE });
		child->setFixedPos({ column * cellW, UI_CSS_LEFT_PERCENTAGE }, { row * cellH, UI_CSS_TOP_PERCENTAGE });
		child->setFixedSize({ cellW, UI_CSS_WIDTH_PERCENTAGE }, { cellH, UI_CSS_HEIGHT_PERCENTAGE });
	}
}

void PCGrid::paint(UIRect client, UIPainterRaw painter)
{
	UIWidget::paint(client, painter);
	PCDrawFace(painter, client, PCFaceOf(getStyleNormal(), client));
}

int32_t PCGrid::getColumnCount() const
{
	return PRIVATE()->Columns;
}

void PCGrid::setColumnCount(int32_t value)
{
	PRIVATE()->Columns = std::max(1, value);
}
#undef PRIVATE
