#include "IGStack.h"

struct IGStackPrivate : UIPrivate
{
	IGFrameStyle Style;
};
#define PRIVATE() ((IGStackPrivate*)m_Private)

IGStack::IGStack(UICanvasRaw canvas)
	:
	UIWidget(canvas)
{
	m_Private = new IGStackPrivate;
}

IGStack::~IGStack()
{
	delete m_Private; m_Private = nullptr;
}

void IGStack::arrange(UIRect client)
{
	(void)client;
	for (size_t i = 0; i < getWidgets().size(); ++i)
	{
		auto child = getWidgets()[i];
		child->setPositionType({UI_CSS_POSITION_ABSOLUTE});
		child->setFixedPos({0.0f, UI_CSS_LEFT_PERCENTAGE}, {0.0f, UI_CSS_TOP_PERCENTAGE});
		child->setFixedSize({100.0f, UI_CSS_WIDTH_PERCENTAGE}, {100.0f, UI_CSS_HEIGHT_PERCENTAGE});
	}
}

void IGStack::paint(UIRect client, UIPainterRaw painter)
{
	UIWidget::paint(client, painter);
	IGDrawBox(painter, client, PRIVATE()->Style.Pen, PRIVATE()->Style.Brush, 2);
}

IGFrameStyle IGStack::getStyle() const
{
	return PRIVATE()->Style;
}

void IGStack::setStyle(IGFrameStyle value)
{
	PRIVATE()->Style = value;
}
#undef PRIVATE
