#include "IGFlow.h"

struct IGFlowPrivate : UIPrivate
{
	IGFrameStyle Style;
};
#define PRIVATE() ((IGFlowPrivate*)m_Private)

IGFlow::IGFlow(UICanvasRaw canvas)
	:
	UIWidget(canvas)
{
	m_Private = new IGFlowPrivate;
}

IGFlow::~IGFlow()
{
	delete m_Private; m_Private = nullptr;
}

void IGFlow::arrange(UIRect client)
{
	(void)client;
	this->setFlexWrap({UI_CSS_FLEX_WRAP_WRAP});
	this->setAlignItems({UI_CSS_ALIGN_ITEMS_FLEX_START});
	this->setAlignContent({UI_CSS_ALIGN_CONTENT_FLEX_START});
	this->setFlexDirection({UI_CSS_FLEX_DIRECTION_ROW});
	this->setJustifyContent({UI_CSS_JUSTIFY_CONTENT_FLEX_START});

	for (size_t i = 0; i < getWidgets().size(); ++i)
	{
		auto child = getWidgets()[i];
		if (child->getFixedWidth().Type == UI_CSS_WIDTH_AUTO) child->setFlexGrow({1.0f, UI_CSS_FLEX_GROW_NUMBER});
		else child->setFlexGrow({0.0f, UI_CSS_FLEX_GROW_NUMBER});
		if (child->getFixedHeight().Type == UI_CSS_HEIGHT_AUTO) child->setAlignSelf({UI_CSS_ALIGN_SELF_STRETCH});
		else child->setAlignSelf({UI_CSS_ALIGN_SELF_FLEX_START});
	}
}

void IGFlow::paint(UIRect client, UIPainterRaw painter)
{
	UIWidget::paint(client, painter);
	IGDrawBox(painter, client, PRIVATE()->Style.Pen, PRIVATE()->Style.Brush, 0);
}

IGFrameStyle IGFlow::getStyle() const
{
	return PRIVATE()->Style;
}

void IGFlow::setStyle(IGFrameStyle value)
{
	PRIVATE()->Style = value;
}
#undef PRIVATE
